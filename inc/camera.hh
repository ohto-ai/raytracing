#pragma once
// camera.hh

#ifndef _OHTOAI_CAMERA_H_
#define _OHTOAI_CAMERA_H_

#include "hittable.hh"
#include "hittable_list.hh"
#include "color.hh"
#include "material.hh"
#include <thread>

namespace ohtoai{
    namespace math {
        class Camera {
        public:
            Camera() = default;

            template<typename WriteColorFunc>
            void render(const HittableList& world, WriteColorFunc func) {
                initialize();
                int thread_count = std::thread::hardware_concurrency();
                int line_count = image_height / thread_count;
                std::vector<std::thread> threads;
                for (int i = 0; i < thread_count; ++i) {
                    auto start_y = i * line_count;
                    auto end_y = (i + 1) * line_count;
                    if (end_y > image_height) {
                        end_y = image_height;
                        break;
                    }
                    threads.emplace_back([&](int start_y, int end_y) {
                        for (int y = start_y; y < end_y; ++y) {
                            const auto height_vec = y * pixel_delta_v;
                            for (int x = 0; x < image_width; ++x) {
                                Color pixel_color {};
                                for (int s = 0; s < samples_per_pixel; ++s) {
                                    auto ray = get_ray(x, y, height_vec);
                                    pixel_color += ray_color(ray, max_depth, world) * 255;
                                }
                                pixel_color /= samples_per_pixel;
                                pixel_color = pixel_color.gamma_correction();
                                func(x, y, pixel_color);
                            }
                        }
                    }, start_y, end_y);
                }
                for (auto& thread : threads) {
                    thread.join();
                }
            }

            Color ray_color(const Ray& light, int depth, const HittableList& world) {
                HitRecord record;
                if (depth <=0 ) {
                    return {};
                }
                if (world.hit(light, make_interval(0.001, ohtoai::math::constants::infinity), record)) {
                    Ray scattered;
                    Color attenuation;
                    if (record.material->scatter(light, record, attenuation, scattered)) {
                        return attenuation * ray_color(scattered, depth - 1, world);
                    } else {
                        return {};
                    }
                }

                // background
                auto unit_direction = light.direction().normalized();
                const auto a = 0.5 * (unit_direction.y() + 1.0);
                auto result = Color::rgb(0xffffff).mix(Color::rgb(0x80B3FF), a).to_unit();
                return result;
            }
        public:
            int image_width = 640;
            int image_height = 360;
            real aspect_ratio = 16.0 / 9.0;
            int samples_per_pixel = 10;
            int max_depth = 10;
            real v_fov = 90;
            Point3 look_from = make_point(0, 0, -1);
            Point3 look_at = make_point(0, 0, 0);
            Vec3 view_up = make_vector(0, 1, 0);

        protected:
            void initialize() {
                image_height = image_width > aspect_ratio ? static_cast<int>(image_width / aspect_ratio) : 1;

                // camera
                const auto focal_length = (look_from - look_at).length();
                const auto theta = degrees_to_radians(v_fov);
                const auto h = tan(theta / 2);
                const auto viewport_height = 2 * h * focal_length;
                const auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
                camera_center = look_from;

                w = (look_from - look_at).normalized();
                u = view_up.cross(w).normalized();
                v = w.cross(u);

                // 计算横纵向量 u 和 v
                const auto viewport_u = viewport_width * u;
                const auto viewport_v = viewport_height * -v;

                // 由像素间距计算横纵增量向量
                pixel_delta_u = viewport_u / image_width;
                pixel_delta_v = viewport_v / image_height;

                // 计算左上角像素的位置
                const auto viewport_upper_left = camera_center - focal_length * w - viewport_u / 2 - viewport_v / 2;
                pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
            }
            Ray get_ray(int x, int y, const Vec3& height_vec) const {
                auto pixel_center = pixel100_loc + (x * pixel_delta_u) + height_vec;
                auto pixel_sample = pixel_center + random_real(-0.5, 0.5) * pixel_delta_u + random_real(-0.5, 0.5) * pixel_delta_v;
                const auto ray_direction = pixel_sample - camera_center;
                return Ray(camera_center, ray_direction);
            }

        protected:
            real focal_length;
            Point3 camera_center;
            Vec3 pixel_delta_u;
            Vec3 pixel_delta_v;
            Point3 pixel100_loc;
            Vec3 u;
            Vec3 v;
            Vec3 w;
        };
    }
}

#endif // !_OHTOAI_CAMERA_H_
