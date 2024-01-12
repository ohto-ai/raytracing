#pragma once
// camera.hh

#ifndef _OHTOAI_CAMERA_H_
#define _OHTOAI_CAMERA_H_

#include "hittable.hh"
#include "color.hh"

namespace ohtoai{
    namespace math {
        template<typename T>
        class Camera {
        public:
            using value_type = T;
            using point_type = Point3<T>;
            using vector_type = Vec3<T>;
            using ray_type = Ray<T>;
            using interval_type = Interval<T>;
            using color_type = Color<T>;
            using hittable_type = Hittable<T>;
            using hit_record_type = HitRecord<T>;

            Camera() = default;

            template<typename WriteColorFunc>
            void render(const hittable_type& world, WriteColorFunc func) {
                initialize();
                for (int y = 0; y < image_height; ++y) {
                    const auto height_vec = y * pixel_delta_v;
                    for (int x = 0; x < image_width; ++x) {
                        auto pixel_center = pixel100_loc + (x * pixel_delta_u) + height_vec;
                        const auto Ray_direction = pixel_center - camera_center;
                        Ray light(camera_center, Ray_direction);
                        auto write_Color = ray_color(light, world);
                        func(x, y, write_Color);
                    }
                }
            }

            color_type ray_color(const ray_type& light, const hittable_type& world) {
                hit_record_type record;
                if (world.hit(light, make_interval(0.0, ohtoai::math::constants::infinity), record)) {
                    return color_type::rgb(0xffffff).mix(color_type(record.normal * 255));
                }

                // background
                auto unit_direction = light.direction().normalized();
                const auto a = 0.5 * (unit_direction.y() + 1.0);
                auto result = color_type::rgb(0xffffff).mix(color_type::rgb(0x7fb2ff), a);
                return result;
            }
        public:
            int image_width = 640;
            int image_height = 360;
            value_type aspect_ratio = 16.0 / 9.0;

        protected:
            void initialize() {
                image_height = image_width > aspect_ratio ? static_cast<int>(image_width / aspect_ratio) : 1;

                // camera
                const auto focal_length = 1.0;
                const auto viewport_height = 2.0;
                const auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
                camera_center = point_type(0, 0, 0);

                // 计算横纵向量 u 和 v
                const auto viewport_u = vector_type(viewport_width, 0, 0);
                const auto viewport_v = vector_type(0, -viewport_height, 0);

                // 由像素间距计算横纵增量向量
                pixel_delta_u = viewport_u / image_width;
                pixel_delta_v = viewport_v / image_height;

                // 计算左上角像素的位置
                const auto viewport_upper_left = camera_center - vector_type(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
                pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
            }
            ray_type get_ray(value_type u, value_type v) const {
                return ray_type(origin_, lower_left_corner_ + u * horizontal_ + v * vertical_ - origin_);
            }

        protected:
            value_type focal_length;
            point_type camera_center;
            vector_type pixel_delta_u;
            vector_type pixel_delta_v;
            point_type pixel100_loc;
        };
    }
}

#endif // !_OHTOAI_CAMERA_H_
