#pragma once
// cuda_scene.hh
// CUDA-compatible (no virtual dispatch, no std::shared_ptr, no std::vector)
// scene representation that mirrors the CPU-side object hierarchy.

#ifndef _OHTOAI_CUDA_SCENE_H_
#define _OHTOAI_CUDA_SCENE_H_

#include "color.hh"
#include "ray.hh"
#include "interval.hh"

namespace ohtoai {
    namespace math {

        // ------------------------------------------------------------------ //
        //  Material
        // ------------------------------------------------------------------ //

        struct CudaMaterial {
            enum class Type : int {
                Lambertian = 0,
                Metal      = 1,
                Dielectric = 2,
            };

            Type  type  = Type::Lambertian;
            Color albedo;       ///< Lambertian / Metal albedo (unit-space, 0-1)
            real  fuzz  = 0;    ///< Metal fuzz factor
            real  ir    = 1.5;  ///< Dielectric index of refraction
        };

        // ------------------------------------------------------------------ //
        //  Sphere
        // ------------------------------------------------------------------ //

        struct CudaSphere {
            Point3 center;
            real   radius     = 1.0;
            int    mat_index  = 0;  ///< index into CudaScene::materials[]
        };

        // ------------------------------------------------------------------ //
        //  Hit record (no shared_ptr – uses a material index)
        // ------------------------------------------------------------------ //

        struct CudaHitRecord {
            Point3 point;
            Vec3   normal;
            real   t         = 0;
            bool   front_face = false;
            int    mat_index  = -1;

            OHTOAI_HOST_DEVICE void set_face_normal(const Ray& light, const Vec3& outward_normal) {
                front_face = light.direction().dot(outward_normal) < 0;
                normal     = front_face ? outward_normal : -outward_normal;
            }
        };

        // ------------------------------------------------------------------ //
        //  Scene (flat arrays – safe to pass directly into a CUDA kernel)
        // ------------------------------------------------------------------ //

        struct CudaScene {
            const CudaSphere*   spheres        = nullptr;
            int                 sphere_count   = 0;
            const CudaMaterial* materials      = nullptr;
            int                 material_count = 0;
        };

        // ------------------------------------------------------------------ //
        //  Camera parameters (POD – no thread / shared_ptr members)
        // ------------------------------------------------------------------ //

        struct CudaCameraParams {
            int    image_width      = 640;
            int    image_height     = 360;
            int    samples_per_pixel = 10;
            int    max_depth        = 10;
            Point3 pixel100_loc;
            Vec3   pixel_delta_u;
            Vec3   pixel_delta_v;
            Point3 camera_center;
            Vec3   defocus_disk_u;
            Vec3   defocus_disk_v;
            real   defocus_angle   = 0;
        };

    } // namespace math
} // namespace ohtoai

#endif // !_OHTOAI_CUDA_SCENE_H_
