// render.cpp
// CPU rendering implementation.
// The ray-tracing algorithm is identical to render.cu; only the
// parallelism mechanism differs (std::thread vs. CUDA threads) and the
// random number generator (std::mt19937 vs. cuRAND).

#include "render.hh"
#include "type_base.hh"

#include <thread>
#include <vector>
#include <cstdint>
#include <cmath>
#include <random>

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
// Thread-local random number generator (mirrors cuRAND per-thread state)
// =========================================================================

static thread_local std::mt19937                          tl_rng{std::random_device{}()};
static thread_local std::uniform_real_distribution<real>  tl_dist{0.0, 1.0};

static inline real cpu_random_real() {
    return tl_dist(tl_rng);
}

static inline real cpu_random_real(real lo, real hi) {
    return lo + (hi - lo) * cpu_random_real();
}

// =========================================================================
// CPU geometry helpers  (mirrors device helpers in render.cu)
// =========================================================================

static Vec3 cpu_random_in_unit_sphere() {
    while (true) {
        Vec3 v(cpu_random_real(-1.0, 1.0),
               cpu_random_real(-1.0, 1.0),
               cpu_random_real(-1.0, 1.0));
        if (v.length2() <= 1.0)
            return v;
    }
}

static Vec3 cpu_random_unit_vector() {
    return cpu_random_in_unit_sphere().normalized();
}

static Vec3 cpu_random_in_unit_disk() {
    while (true) {
        Vec3 v(cpu_random_real(-1.0, 1.0),
               cpu_random_real(-1.0, 1.0),
               0.0);
        if (v.length2() <= 1.0)
            return v;
    }
}

// =========================================================================
// CPU sphere intersection  (mirrors cuda_hit_sphere in render.cu)
// =========================================================================

static bool cpu_hit_sphere(const CudaSphere& sphere,
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

    real t = (-b - std::sqrt(delta)) / (2.0 * a);
    if (t <= t_min || t >= t_max) {
        t = (-b + std::sqrt(delta)) / (2.0 * a);
        if (t <= t_min || t >= t_max)
            return false;
    }

    rec.t         = t;
    rec.point     = light.at(t);
    rec.mat_index = sphere.mat_index;
    rec.set_face_normal(light, (rec.point - sphere.center) / sphere.radius);
    return true;
}

// =========================================================================
// CPU scene intersection  (mirrors cuda_hit_scene in render.cu)
// =========================================================================

static bool cpu_hit_scene(const CudaScene& scene,
                           const Ray& light,
                           real t_min, real t_max,
                           CudaHitRecord& rec)
{
    CudaHitRecord temp;
    bool hit_anything   = false;
    real closest_so_far = t_max;

    for (int i = 0; i < scene.sphere_count; ++i) {
        if (cpu_hit_sphere(scene.spheres[i], light, t_min, closest_so_far, temp)) {
            hit_anything   = true;
            closest_so_far = temp.t;
            rec            = temp;
        }
    }
    return hit_anything;
}

// =========================================================================
// CPU scatter  (mirrors cuda_scatter in render.cu)
// =========================================================================

static bool cpu_scatter(const CudaMaterial& mat,
                         const Ray& light,
                         const CudaHitRecord& rec,
                         Color& attenuation,
                         Ray& scattered)
{
    switch (mat.type) {
        case CudaMaterial::Type::Lambertian: {
            Vec3 scatter_dir = rec.normal + cpu_random_unit_vector();
            if (scatter_dir.near_zero())
                scatter_dir = rec.normal;
            scattered   = make_ray(rec.point, scatter_dir);
            attenuation = mat.albedo;
            return true;
        }
        case CudaMaterial::Type::Metal: {
            Vec3 reflected = light.direction().normalized().reflect(rec.normal);
            scattered   = make_ray(rec.point, reflected + mat.fuzz * cpu_random_in_unit_sphere());
            attenuation = mat.albedo;
            return true;
        }
        case CudaMaterial::Type::Dielectric: {
            attenuation = Color(1.0, 1.0, 1.0);
            real refraction_ratio = rec.front_face ? (1.0 / mat.ir) : mat.ir;

            Vec3 unit_dir  = light.direction().normalized();
            real cos_theta = std::min(-unit_dir.dot(rec.normal), 1.0);
            real sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            real r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
            r0 = r0 * r0;
            real reflectance = r0 + (1 - r0) * std::pow((1 - cos_theta), 5);

            Vec3 direction;
            if (cannot_refract || reflectance > cpu_random_real()) {
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
// CPU ray colour  (mirrors cuda_ray_color in render.cu)
// =========================================================================

static Color cpu_ray_color(const Ray& light,
                            int depth,
                            const CudaScene& scene)
{
    Ray   current_ray   = light;
    Color accumulated   = Color(1.0, 1.0, 1.0);

    for (int d = 0; d < depth; ++d) {
        CudaHitRecord rec;
        if (cpu_hit_scene(scene, current_ray, 0.001, infinity, rec)) {
            Ray   scattered;
            Color attenuation;
            if (rec.mat_index >= 0 && rec.mat_index < scene.material_count &&
                cpu_scatter(scene.materials[rec.mat_index], current_ray, rec, attenuation, scattered))
            {
                accumulated = accumulated * attenuation;
                current_ray = scattered;
            } else {
                return Color(0.0, 0.0, 0.0);
            }
        } else {
            // Background gradient (same as Camera::ray_color)
            Vec3 unit_dir = current_ray.direction().normalized();
            const real a  = 0.5 * (unit_dir.y() + 1.0);
            Color background = Color::rgb(0xffffff).mix(Color::rgb(0x80B3FF), a).to_unit();
            return accumulated * background;
        }
    }
    return Color(0.0, 0.0, 0.0);
}

// =========================================================================
// Render one horizontal band of pixels  (called from each worker thread)
// =========================================================================

static void render_band(const CudaCameraParams& cam,
                         const CudaScene& scene,
                         uint8_t* output,
                         int y_begin, int y_end)
{
    auto clamp_byte = [](real v) -> uint8_t {
        return static_cast<uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v));
    };

    for (int y = y_begin; y < y_end; ++y) {
        const Vec3 height_vec = static_cast<real>(y) * cam.pixel_delta_v;
        for (int x = 0; x < cam.image_width; ++x) {
            Color pixel_color{};

            for (int s = 0; s < cam.samples_per_pixel; ++s) {
                // Same sampling as Camera::get_ray / pixel_sample_square
                Vec3 pixel_center  = cam.pixel100_loc
                                   + (static_cast<real>(x) * cam.pixel_delta_u)
                                   + height_vec;
                Vec3 pixel_sample  = pixel_center
                                   + cpu_random_real(-0.5, 0.5) * cam.pixel_delta_u
                                   + cpu_random_real(-0.5, 0.5) * cam.pixel_delta_v;

                Point3 ray_origin;
                if (cam.defocus_angle <= 0) {
                    ray_origin = cam.camera_center;
                } else {
                    Vec3 p    = cpu_random_in_unit_disk();
                    ray_origin = cam.camera_center
                               + p.x() * cam.defocus_disk_u
                               + p.y() * cam.defocus_disk_v;
                }

                Ray ray = make_ray(ray_origin, pixel_sample - ray_origin);
                pixel_color += cpu_ray_color(ray, cam.max_depth, scene).to_ununit();
            }

            pixel_color /= static_cast<real>(cam.samples_per_pixel);
            pixel_color  = pixel_color.gamma_correction();

            const int idx    = (y * cam.image_width + x) * 4;
            output[idx + 0]  = clamp_byte(pixel_color.red());
            output[idx + 1]  = clamp_byte(pixel_color.green());
            output[idx + 2]  = clamp_byte(pixel_color.blue());
            output[idx + 3]  = 255;
        }
    }
}

// =========================================================================
// Public host function declared in render.hh
// =========================================================================

namespace ohtoai {
namespace math {

void render_cpu(const CudaCameraParams& cam,
                const CudaScene&        scene,
                uint8_t*                output)
{
    const int thread_count = std::max(1, static_cast<int>(std::thread::hardware_concurrency()));
    const int band_height  = cam.image_height / thread_count;

    std::vector<std::thread> threads;
    threads.reserve(thread_count);

    for (int i = 0; i < thread_count; ++i) {
        const int y_begin = i * band_height;
        int y_end         = y_begin + band_height;
        if (i == thread_count - 1)
            y_end = cam.image_height;   // last thread covers any remainder
        threads.emplace_back(render_band, std::cref(cam), std::cref(scene),
                             output, y_begin, y_end);
    }

    for (auto& t : threads)
        t.join();
}

} // namespace math
} // namespace ohtoai
