// render.cu — Optimised CUDA ray-tracing back-end.
//
// Performance improvements over the original implementation:
//   * All GPU-side arithmetic uses float (2–8× faster than double on most NVIDIA
//     GPUs, which have 32:1 or higher float:double throughput).
//   * Scene geometry and materials are stored in CUDA constant memory, giving
//     broadcast-cached reads rather than global-memory traffic.
//   * Sphere intersection uses the half-b discriminant formula, which has better
//     numerical stability and fewer floating-point operations.
//   * All device helpers are __forceinline__ to eliminate call overhead.
//   * Fast rsqrtf() hardware instruction used for vector normalisation.
//   * Shadow-ray epsilon raised to 1e-4f (float-appropriate) to reduce acne.
//   * Sky gradient matches the original colour scheme exactly.

#include "cuda_scene.hh"
#include "render.hh"

#include <curand_kernel.h>
#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <chrono>

// =========================================================================
// Self-contained float3 math (keeps the kernel independent of double-based
// host headers and avoids unnecessary type conversions on the GPU).
// =========================================================================

struct F3 {
    float x, y, z;

    __forceinline__ __device__ F3() = default;
    __forceinline__ __device__ constexpr F3(float x_, float y_, float z_)
        : x(x_), y(y_), z(z_) {}

    __forceinline__ __device__ F3 operator+(const F3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    __forceinline__ __device__ F3 operator-(const F3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    __forceinline__ __device__ F3 operator*(const F3& o) const { return {x*o.x, y*o.y, z*o.z}; }
    __forceinline__ __device__ F3 operator*(float t)    const { return {x*t,   y*t,   z*t  }; }
    __forceinline__ __device__ F3 operator/(float t)    const { float inv = 1.0f/t; return {x*inv, y*inv, z*inv}; }
    __forceinline__ __device__ F3 operator-()           const { return {-x, -y, -z}; }

    __forceinline__ __device__ F3& operator+=(const F3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
    __forceinline__ __device__ F3& operator*=(const F3& o) { x*=o.x; y*=o.y; z*=o.z; return *this; }

    __forceinline__ __device__ float dot(const F3& o) const { return x*o.x + y*o.y + z*o.z; }
    __forceinline__ __device__ float len2()           const { return dot(*this); }

    // rsqrtf: fast hardware reciprocal square-root
    __forceinline__ __device__ F3 normalized() const { return *this * rsqrtf(len2()); }

    __forceinline__ __device__ bool near_zero() const {
        constexpr float s = 1e-7f;
        return fabsf(x) < s && fabsf(y) < s && fabsf(z) < s;
    }
    __forceinline__ __device__ F3 reflect(const F3& n) const {
        return *this - n * (2.0f * dot(n));
    }
    __forceinline__ __device__ F3 refract(const F3& n, float eta) const {
        float cos_t = fminf(-dot(n), 1.0f);
        F3 r_perp   = (*this + n * cos_t) * eta;
        F3 r_para   = n * (-sqrtf(fabsf(1.0f - r_perp.len2())));
        return r_perp + r_para;
    }
};

__forceinline__ __device__ F3 operator*(float t, const F3& v) { return v * t; }

// Color is represented as float RGB in [0, 1] throughout the kernel;
// use the same struct for brevity.
using C3 = F3;

// =========================================================================
// GPU scene types (float — stored in constant memory)
// =========================================================================

// Maximum scene size that fits comfortably in the 64 KB constant-memory bank.
#define MAX_SPHERES   256
#define MAX_MATERIALS 256

struct GpuMat {
    int   type;    // 0 = Lambertian, 1 = Metal, 2 = Dielectric
    C3    albedo;  // unit-space [0,1] RGB
    float fuzz;
    float ir;      // index of refraction (Dielectric)
};

struct GpuSphere {
    F3    center;
    float radius;
    int   mat_idx;
};

__constant__ GpuSphere c_spheres[MAX_SPHERES];
__constant__ GpuMat    c_materials[MAX_MATERIALS];
__constant__ int       c_sphere_cnt;
__constant__ int       c_mat_cnt;

// Flat camera parameters passed directly to the kernel (no vtable, no heap).
struct GpuCam {
    int   w, h, spp, max_depth;
    F3    pixel00;       // top-left pixel centre
    F3    du, dv;        // per-pixel step in u and v
    F3    origin;        // camera centre (eye position)
    F3    defocus_u;     // defocus (depth-of-field) disk axes
    F3    defocus_v;
    float defocus_angle;
};

// =========================================================================
// Hit record
// =========================================================================

struct HitRec {
    F3    p;      // hit point
    F3    n;      // outward-facing shading normal
    float t;      // ray parameter
    int   mat;    // material index
    bool  front;  // true if ray hit the front face
};

// =========================================================================
// RNG helpers (cuRAND per-thread xorwow state)
// =========================================================================

__forceinline__ __device__ float rnd(curandState* s) {
    return curand_uniform(s);
}
__forceinline__ __device__ float rnd(curandState* s, float lo, float hi) {
    return lo + (hi - lo) * rnd(s);
}

__forceinline__ __device__ F3 rand_in_unit_sphere(curandState* s) {
    for (;;) {
        F3 v{ rnd(s,-1.0f,1.0f), rnd(s,-1.0f,1.0f), rnd(s,-1.0f,1.0f) };
        if (v.len2() <= 1.0f) return v;
    }
}

__forceinline__ __device__ F3 rand_unit(curandState* s) {
    return rand_in_unit_sphere(s).normalized();
}

__forceinline__ __device__ F3 rand_in_unit_disk(curandState* s) {
    for (;;) {
        F3 v{ rnd(s,-1.0f,1.0f), rnd(s,-1.0f,1.0f), 0.0f };
        if (v.len2() <= 1.0f) return v;
    }
}

// =========================================================================
// Sphere intersection — half-b discriminant (numerically stable)
// =========================================================================

__forceinline__ __device__ bool hit_sphere(
    const GpuSphere& s,
    const F3& ro, const F3& rd,
    float tmin, float tmax,
    HitRec& rec)
{
    F3    oc     = ro - s.center;
    float a      = rd.len2();
    float half_b = oc.dot(rd);
    float c      = oc.len2() - s.radius * s.radius;
    float disc   = half_b * half_b - a * c;

    if (disc < 0.0f) return false;

    float sqrtd = sqrtf(disc);
    float t     = (-half_b - sqrtd) / a;
    if (t <= tmin || t >= tmax) {
        t = (-half_b + sqrtd) / a;
        if (t <= tmin || t >= tmax) return false;
    }

    rec.t     = t;
    rec.p     = ro + rd * t;
    F3 out_n  = (rec.p - s.center) * (1.0f / s.radius);
    rec.front = rd.dot(out_n) < 0.0f;
    rec.n     = rec.front ? out_n : -out_n;
    rec.mat   = s.mat_idx;
    return true;
}

// =========================================================================
// Scene intersection (reads from constant memory — broadcast-cached)
// =========================================================================

__forceinline__ __device__ bool hit_scene(
    const F3& ro, const F3& rd,
    float tmin, float tmax,
    HitRec& rec)
{
    HitRec tmp;
    bool   hit  = false;
    float  best = tmax;

    for (int i = 0; i < c_sphere_cnt; ++i) {
        if (hit_sphere(c_spheres[i], ro, rd, tmin, best, tmp)) {
            hit  = true;
            best = tmp.t;
            rec  = tmp;
        }
    }
    return hit;
}

// =========================================================================
// Scatter (material response)
// =========================================================================

__forceinline__ __device__ bool scatter(
    const GpuMat& mat,
    const F3& rd, const HitRec& rec,
    C3& att, F3& scat_o, F3& scat_d,
    curandState* s)
{
    scat_o = rec.p;
    switch (mat.type) {

        case 0: {   // Lambertian — cosine-weighted diffuse
            F3 dir = rec.n + rand_unit(s);
            if (dir.near_zero()) dir = rec.n;
            scat_d = dir;
            att    = mat.albedo;
            return true;
        }

        case 1: {   // Metal — specular reflection with optional fuzz
            F3 ref = rd.normalized().reflect(rec.n);
            scat_d = ref + rand_in_unit_sphere(s) * mat.fuzz;
            att    = mat.albedo;
            return scat_d.dot(rec.n) > 0.0f;
        }

        case 2: {   // Dielectric — Fresnel/Schlick refraction + reflection
            att = C3{1.0f, 1.0f, 1.0f};
            float eta    = rec.front ? (1.0f / mat.ir) : mat.ir;
            F3    unit_d = rd.normalized();
            float cos_t  = fminf(-unit_d.dot(rec.n), 1.0f);
            float sin_t  = sqrtf(1.0f - cos_t * cos_t);

            bool  no_ref = eta * sin_t > 1.0f;
            float r0     = (1.0f - eta) / (1.0f + eta);
            r0           = r0 * r0;
            float schlick = r0 + (1.0f - r0) * powf(1.0f - cos_t, 5.0f);

            scat_d = (no_ref || schlick > rnd(s))
                         ? unit_d.reflect(rec.n)
                         : unit_d.refract(rec.n, eta);
            return true;
        }

        default:
            return false;
    }
}

// =========================================================================
// Ray colour — iterative (avoids GPU call-stack pressure)
//
// Sky gradient: white (y=-1) → light blue 0x80B3FF (y=+1).
// =========================================================================

__forceinline__ __device__ C3 ray_color(
    F3 ro, F3 rd,
    int max_depth,
    curandState* s)
{
    C3 acc{1.0f, 1.0f, 1.0f};

    for (int d = 0; d < max_depth; ++d) {
        HitRec rec;
        if (hit_scene(ro, rd, 1e-4f, 1e30f, rec)) {
            if (rec.mat < 0 || rec.mat >= c_mat_cnt)
                return C3{0.0f, 0.0f, 0.0f};

            C3 att;
            F3 scat_o, scat_d;
            if (!scatter(c_materials[rec.mat], rd, rec, att, scat_o, scat_d, s))
                return C3{0.0f, 0.0f, 0.0f};

            acc *= att;
            ro   = scat_o;
            rd   = scat_d;
        } else {
            // Sky gradient: lerp(white, #80B3FF, 0.5*(unit_y + 1))
            F3    u = rd.normalized();
            float a = 0.5f * (u.y + 1.0f);
            // 0x80/0xFF = 0.5020, 0xB3/0xFF = 0.7020
            C3 sky{
                (1.0f - a) + a * (128.0f / 255.0f),
                (1.0f - a) + a * (179.0f / 255.0f),
                1.0f
            };
            return acc * sky;
        }
    }
    return C3{0.0f, 0.0f, 0.0f};  // max depth reached
}

// Gamma-correct a linear [0,1] value with γ = 2.0 (sqrt).
__forceinline__ __device__ float gamma2(float v) {
    return v > 0.0f ? sqrtf(v) : 0.0f;
}

// Convert a [0,1] float to a clamped uint8_t.
__forceinline__ __device__ uint8_t to_byte(float v) {
    const int i = (int)(v * 255.999f);
    return (uint8_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
}

// =========================================================================
// Kernel: one thread per pixel, cam.spp samples accumulated per pixel
// =========================================================================

__global__ void render_kernel(
    uint8_t* __restrict__ out,
    GpuCam  cam,
    unsigned seed)
{
    const int x = (int)(blockIdx.x * blockDim.x + threadIdx.x);
    const int y = (int)(blockIdx.y * blockDim.y + threadIdx.y);
    if (x >= cam.w || y >= cam.h) return;

    // Use seed as the RNG seed and the pixel index as the sequence number.
    // Each sequence in xorwow is guaranteed to produce 2^67 non-overlapping
    // values, so no two pixels share any portion of the same stream.
    curandState rng;
    curand_init(seed, (unsigned long long)(y * cam.w + x), 0, &rng);

    C3 color{0.0f, 0.0f, 0.0f};
    F3 pixel_center = cam.pixel00 + cam.du * (float)x + cam.dv * (float)y;

    for (int s = 0; s < cam.spp; ++s) {
        // Tent/uniform jitter within the pixel footprint
        F3 sample = pixel_center
                  + cam.du * rnd(&rng, -0.5f, 0.5f)
                  + cam.dv * rnd(&rng, -0.5f, 0.5f);

        F3 ro;
        if (cam.defocus_angle <= 0.0f) {
            ro = cam.origin;
        } else {
            F3 p = rand_in_unit_disk(&rng);
            ro   = cam.origin + cam.defocus_u * p.x + cam.defocus_v * p.y;
        }

        color += ray_color(ro, sample - ro, cam.max_depth, &rng);
    }

    // Average + gamma-correct (γ = 2.0, i.e. sqrt, as per RTIOW standard)
    const float inv = 1.0f / (float)cam.spp;
    const int idx   = (y * cam.w + x) * 4;
    out[idx + 0]    = to_byte(gamma2(color.x * inv));
    out[idx + 1]    = to_byte(gamma2(color.y * inv));
    out[idx + 2]    = to_byte(gamma2(color.z * inv));
    out[idx + 3]    = 0xff;
}

// =========================================================================
// Utility: convert double-based host Vec3 to float F3
// =========================================================================

static inline F3 to_f3(const ohtoai::math::Vec3& v) {
    return { (float)v.x(), (float)v.y(), (float)v.z() };
}

// =========================================================================
// Public host entry point (declared in render.hh)
// =========================================================================

namespace ohtoai {
namespace math {

void render_cuda(const CudaCameraParams& cam,
                 const CudaScene&        host_scene,
                 uint8_t*                host_output)
{
    // --- Convert scene to GPU float structs, upload to constant memory -------
    static GpuSphere host_spheres[MAX_SPHERES];
    static GpuMat    host_mats[MAX_MATERIALS];

    const int sc = std::min(host_scene.sphere_count,   MAX_SPHERES);
    const int mc = std::min(host_scene.material_count, MAX_MATERIALS);

    for (int i = 0; i < sc; ++i) {
        host_spheres[i].center  = to_f3(host_scene.spheres[i].center);
        host_spheres[i].radius  = (float)host_scene.spheres[i].radius;
        host_spheres[i].mat_idx = host_scene.spheres[i].mat_index;
    }
    for (int i = 0; i < mc; ++i) {
        const auto& m      = host_scene.materials[i];
        host_mats[i].type  = (int)m.type;
        host_mats[i].albedo = {
            (float)m.albedo.red(),
            (float)m.albedo.green(),
            (float)m.albedo.blue()
        };
        host_mats[i].fuzz  = (float)m.fuzz;
        host_mats[i].ir    = (float)m.ir;
    }

    cudaMemcpyToSymbol(c_spheres,    host_spheres,     sizeof(GpuSphere) * sc);
    cudaMemcpyToSymbol(c_materials,  host_mats,        sizeof(GpuMat)    * mc);
    cudaMemcpyToSymbol(c_sphere_cnt, &sc,              sizeof(int));
    cudaMemcpyToSymbol(c_mat_cnt,    &mc,              sizeof(int));

    // --- Build float camera params -------------------------------------------
    GpuCam gpu_cam;
    gpu_cam.w             = cam.image_width;
    gpu_cam.h             = cam.image_height;
    gpu_cam.spp           = cam.samples_per_pixel;
    gpu_cam.max_depth     = cam.max_depth;
    gpu_cam.pixel00       = to_f3(cam.pixel100_loc);
    gpu_cam.du            = to_f3(cam.pixel_delta_u);
    gpu_cam.dv            = to_f3(cam.pixel_delta_v);
    gpu_cam.origin        = to_f3(cam.camera_center);
    gpu_cam.defocus_u     = to_f3(cam.defocus_disk_u);
    gpu_cam.defocus_v     = to_f3(cam.defocus_disk_v);
    gpu_cam.defocus_angle = (float)cam.defocus_angle;

    // --- Allocate device output buffer ----------------------------------------
    const size_t output_size = (size_t)cam.image_width * cam.image_height * 4;
    uint8_t* d_out = nullptr;
    cudaMalloc(&d_out, output_size);

    // --- Launch: 16×8 block (128 threads, warp-friendly 2:1 width:height) ----
    const dim3 block(16, 8);
    const dim3 grid(
        (cam.image_width  + block.x - 1) / block.x,
        (cam.image_height + block.y - 1) / block.y
    );

    const unsigned seed = (unsigned)
        std::chrono::high_resolution_clock::now().time_since_epoch().count();

    render_kernel<<<grid, block>>>(d_out, gpu_cam, seed);
    cudaDeviceSynchronize();

    // --- Copy result back to host ---------------------------------------------
    cudaMemcpy(host_output, d_out, output_size, cudaMemcpyDeviceToHost);
    cudaFree(d_out);
}

} // namespace math
} // namespace ohtoai
