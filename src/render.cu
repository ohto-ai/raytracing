// render.cu
// CUDA rendering implementation.
// The ray-tracing algorithm mirrors the CPU version in camera.hh exactly;
// only the parallelism and RNG differ (GPU threads vs. std::thread, cuRAND vs. std::mt19937).

#include "cuda_scene.hh"
#include "render.hh"

#include <curand_kernel.h>
#include <cstdint>
#include <cstdio>
#include <chrono>

using ohtoai::real;
using ohtoai::math::Color;
using ohtoai::math::Ray;
using ohtoai::math::Vec3;
using ohtoai::math::Point3;
using ohtoai::math::CudaScene;
using ohtoai::math::CudaSphere;
using ohtoai::math::CudaMaterial;
using ohtoai::math::CudaHitRecord;
using ohtoai::math::CudaCameraParams;
using ohtoai::math::make_ray;
using ohtoai::math::make_interval;
using ohtoai::math::make_vector;
using ohtoai::math::constants::infinity;

// =========================================================================
// Device-side random utilities (cuRAND wrappers)
// =========================================================================

__device__ inline real cuda_random_real(curandState* state) {
    return curand_uniform_double(state);
}

__device__ inline real cuda_random_real(curandState* state, real lo, real hi) {
    return lo + (hi - lo) * cuda_random_real(state);
}

// =========================================================================
// Device-side geometry helpers
// =========================================================================

__device__ Vec3 cuda_random_in_unit_sphere(curandState* state) {
    while (true) {
        Vec3 v(cuda_random_real(state, -1.0, 1.0),
               cuda_random_real(state, -1.0, 1.0),
               cuda_random_real(state, -1.0, 1.0));
        if (v.length2() <= 1.0)
            return v;
    }
}

__device__ Vec3 cuda_random_unit_vector(curandState* state) {
    return cuda_random_in_unit_sphere(state).normalized();
}

__device__ Vec3 cuda_random_in_unit_disk(curandState* state) {
    while (true) {
        Vec3 v(cuda_random_real(state, -1.0, 1.0),
               cuda_random_real(state, -1.0, 1.0),
               0.0);
        if (v.length2() <= 1.0)
            return v;
    }
}

// =========================================================================
// Device-side sphere intersection (mirrors Sphere::hit)
// =========================================================================

__device__ bool cuda_hit_sphere(const CudaSphere& sphere,
                                 const Ray& light,
                                 real t_min, real t_max,
                                 CudaHitRecord& rec)
{
    Vec3 origin = light.origin() - sphere.center;
    const real a      = light.direction().dot(light.direction());
    const real b      = 2.0 * origin.dot(light.direction());
    const real c      = origin.dot(origin) - sphere.radius * sphere.radius;
    const real delta  = b * b - 4 * a * c;

    if (delta < 0)
        return false;

    real t = (-b - sqrt(delta)) / (2.0 * a);
    if (t <= t_min || t >= t_max) {
        t = (-b + sqrt(delta)) / (2.0 * a);
        if (t <= t_min || t >= t_max)
            return false;
    }

    rec.t        = t;
    rec.point    = light.at(t);
    rec.mat_index = sphere.mat_index;
    rec.set_face_normal(light, (rec.point - sphere.center) / sphere.radius);
    return true;
}

// =========================================================================
// Device-side scene intersection (mirrors HittableList::hit)
// =========================================================================

__device__ bool cuda_hit_scene(const CudaScene& scene,
                                const Ray& light,
                                real t_min, real t_max,
                                CudaHitRecord& rec)
{
    CudaHitRecord temp;
    bool hit_anything      = false;
    real closest_so_far    = t_max;

    for (int i = 0; i < scene.sphere_count; ++i) {
        if (cuda_hit_sphere(scene.spheres[i], light, t_min, closest_so_far, temp)) {
            hit_anything    = true;
            closest_so_far  = temp.t;
            rec             = temp;
        }
    }
    return hit_anything;
}

// =========================================================================
// Device-side scatter (mirrors Material subclasses)
// =========================================================================

__device__ bool cuda_scatter(const CudaMaterial& mat,
                              const Ray& light,
                              const CudaHitRecord& rec,
                              Color& attenuation,
                              Ray& scattered,
                              curandState* state)
{
    switch (mat.type) {
        case CudaMaterial::Type::Lambertian: {
            Vec3 scatter_dir = rec.normal + cuda_random_unit_vector(state);
            if (scatter_dir.near_zero())
                scatter_dir = rec.normal;
            scattered   = make_ray(rec.point, scatter_dir);
            attenuation = mat.albedo;
            return true;
        }
        case CudaMaterial::Type::Metal: {
            Vec3 reflected = light.direction().normalized().reflect(rec.normal);
            scattered   = make_ray(rec.point, reflected + mat.fuzz * cuda_random_in_unit_sphere(state));
            attenuation = mat.albedo;
            return true;
        }
        case CudaMaterial::Type::Dielectric: {
            attenuation = Color(1.0, 1.0, 1.0);
            real refraction_ratio = rec.front_face ? (1.0 / mat.ir) : mat.ir;

            Vec3 unit_dir  = light.direction().normalized();
            real cos_theta = fmin(-unit_dir.dot(rec.normal), 1.0);
            real sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            // Schlick approximation
            real r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
            r0 = r0 * r0;
            real reflectance = r0 + (1 - r0) * pow((1 - cos_theta), 5);

            Vec3 direction;
            if (cannot_refract || reflectance > cuda_random_real(state)) {
                direction = unit_dir.reflect(rec.normal);
            } else {
                direction = unit_dir.refract(rec.normal, refraction_ratio);
            }
            scattered = make_ray(rec.point, direction);
            return true;
        }
        default:
            return false;
    }
}

// =========================================================================
// Device-side ray colour (mirrors Camera::ray_color)
// =========================================================================

__device__ Color cuda_ray_color(const Ray& light,
                                 int depth,
                                 const CudaScene& scene,
                                 curandState* state)
{
    Ray   current_ray   = light;
    Color accumulated   = Color(1.0, 1.0, 1.0);

    for (int d = 0; d < depth; ++d) {
        CudaHitRecord rec;
        if (cuda_hit_scene(scene, current_ray, 0.001, infinity, rec)) {
            Ray   scattered;
            Color attenuation;
            if (rec.mat_index >= 0 && rec.mat_index < scene.material_count &&
                cuda_scatter(scene.materials[rec.mat_index], current_ray, rec, attenuation, scattered, state))
            {
                accumulated = accumulated * attenuation;
                current_ray = scattered;
            } else {
                // Absorbed – contribute nothing
                return Color(0.0, 0.0, 0.0);
            }
        } else {
            // Background gradient (same as CPU version)
            Vec3 unit_dir = current_ray.direction().normalized();
            const real a  = 0.5 * (unit_dir.y() + 1.0);
            Color background = Color::rgb(0xffffff).mix(Color::rgb(0x80B3FF), a).to_unit();
            return accumulated * background;
        }
    }
    // Max depth reached – return black
    return Color(0.0, 0.0, 0.0);
}

// =========================================================================
// Kernel: one thread per pixel, multiple samples per pixel
// =========================================================================

__global__ void render_kernel(uint8_t*              output,
                               CudaCameraParams      cam,
                               CudaScene             scene,
                               unsigned int          seed)
{
    const int x = blockIdx.x * blockDim.x + threadIdx.x;
    const int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= cam.image_width || y >= cam.image_height)
        return;

    // Initialise per-thread cuRAND state
    curandState rng_state;
    curand_init(seed, y * cam.image_width + x, 0, &rng_state);

    Color pixel_color{};

    // Pixel center is constant across all samples – compute once.
    const Vec3 height_vec  = static_cast<real>(y) * cam.pixel_delta_v;
    const Vec3 pixel_center = cam.pixel100_loc
                            + (static_cast<real>(x) * cam.pixel_delta_u)
                            + height_vec;

    for (int s = 0; s < cam.samples_per_pixel; ++s) {
        // Sample offset within pixel (same as Camera::pixel_sample_square)
        Vec3 pixel_sample = pixel_center
                          + cuda_random_real(&rng_state, -0.5, 0.5) * cam.pixel_delta_u
                          + cuda_random_real(&rng_state, -0.5, 0.5) * cam.pixel_delta_v;

        Point3 ray_origin;
        if (cam.defocus_angle <= 0) {
            ray_origin = cam.camera_center;
        } else {
            Vec3 p    = cuda_random_in_unit_disk(&rng_state);
            ray_origin = cam.camera_center
                        + p.x() * cam.defocus_disk_u
                        + p.y() * cam.defocus_disk_v;
        }

        Ray ray = make_ray(ray_origin, pixel_sample - ray_origin);

        Color sample_color = cuda_ray_color(ray, cam.max_depth, scene, &rng_state).to_ununit();
        pixel_color += sample_color;
    }

    pixel_color /= static_cast<real>(cam.samples_per_pixel);
    pixel_color  = pixel_color.gamma_correction();

    // Write RGBA to output buffer
    const int idx       = (y * cam.image_width + x) * 4;
    output[idx + 0]     = static_cast<uint8_t>(pixel_color.red()   < 0 ? 0 : (pixel_color.red()   > 255 ? 255 : pixel_color.red()));
    output[idx + 1]     = static_cast<uint8_t>(pixel_color.green() < 0 ? 0 : (pixel_color.green() > 255 ? 255 : pixel_color.green()));
    output[idx + 2]     = static_cast<uint8_t>(pixel_color.blue()  < 0 ? 0 : (pixel_color.blue()  > 255 ? 255 : pixel_color.blue()));
    output[idx + 3]     = 255;
}

// =========================================================================
// Public host function declared in render.hh
// =========================================================================

namespace ohtoai {
namespace math {

void render_cuda(const CudaCameraParams& cam,
                 const CudaScene&        host_scene,
                 uint8_t*                host_output)
{
    // --- upload scene data ------------------------------------------------
    CudaSphere*   d_spheres   = nullptr;
    CudaMaterial* d_materials = nullptr;

    cudaMalloc(&d_spheres,   sizeof(CudaSphere)   * host_scene.sphere_count);
    cudaMalloc(&d_materials, sizeof(CudaMaterial) * host_scene.material_count);

    cudaMemcpy(d_spheres,   host_scene.spheres,
               sizeof(CudaSphere)   * host_scene.sphere_count,   cudaMemcpyHostToDevice);
    cudaMemcpy(d_materials, host_scene.materials,
               sizeof(CudaMaterial) * host_scene.material_count, cudaMemcpyHostToDevice);

    CudaScene d_scene { d_spheres, host_scene.sphere_count,
                        d_materials, host_scene.material_count };

    // --- allocate output buffer -------------------------------------------
    const int   pixel_count  = cam.image_width * cam.image_height;
    const size_t output_size = pixel_count * 4;   // RGBA

    uint8_t* d_output = nullptr;
    cudaMalloc(&d_output, output_size);
    cudaMemset(d_output, 0, output_size);

    // --- launch kernel ----------------------------------------------------
    const dim3 threads(16, 16);
    const dim3 blocks((cam.image_width  + threads.x - 1) / threads.x,
                      (cam.image_height + threads.y - 1) / threads.y);

    render_kernel<<<blocks, threads>>>(d_output, cam, d_scene,
                                       static_cast<unsigned int>(
                                           std::chrono::high_resolution_clock::now()
                                               .time_since_epoch().count()));
    cudaDeviceSynchronize();

    // --- retrieve results -------------------------------------------------
    cudaMemcpy(host_output, d_output, output_size, cudaMemcpyDeviceToHost);

    // --- cleanup ----------------------------------------------------------
    cudaFree(d_output);
    cudaFree(d_spheres);
    cudaFree(d_materials);
}

} // namespace math
} // namespace ohtoai
