#include "ray.hh"
#include "color.hh"
#include "sphere.hh"
#include "hittable_list.hh"
#include <spdlog/fmt/fmt.h>
#include <chrono>
#include <thread>
#if defined(_MSC_VER) && !defined(__clang__)
#include <easyx.h>
#define USE_EASYX true
#else
#include <fstream>
#include <svpng/svpng.inc>
#define USE_EASYX false
#endif

using value_type = double;
using Ray = ohtoai::math::Ray<value_type>;
using Color = ohtoai::color::Color<value_type>;
using Point3 = ohtoai::math::Point3<value_type>;
using Vec3 = ohtoai::math::Vec3<value_type>;
using HitRecord = ohtoai::math::HitRecord<value_type>;
using Hittable = ohtoai::math::Hittable<value_type>;
using HittableList = ohtoai::math::HittableList<value_type>;
using Sphere = ohtoai::math::Sphere<value_type>;

Color ray_color(const Ray& light, const Hittable& world) {
    HitRecord record;
    if (world.hit(light, 0, ohtoai::math::constants::infinity, record)) {
        return 0.5 * (Color(record.normal) + Color(1, 1, 1)) * 255;
    }

    // background
    auto unit_direction = light.direction().normalized();
    const auto a = 0.5 * (unit_direction.y() + 1.0);
    auto Result = Color::rgb(0xffffff).mix(Color::rgb(0x7fb2ff), a);
    return Result;
}


int main() {

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 640;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // camera
    const auto focal_length = 1.0;
    const auto viewport_height = 2.0;
    const auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
    const auto camera_center = Point3(0, 0, 0);

    // 计算横纵向量 u 和 v
    const auto viewport_u = Vec3(viewport_width, 0, 0);
    const auto viewport_v = Vec3(0, -viewport_height, 0);

    // 由像素间距计算横纵增量向量
    const auto pixel_delta_u = viewport_u / image_width;
    const auto pixel_delta_v = viewport_v / image_height;

    // 计算左上角像素的位置
    const auto viewport_upper_left = camera_center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    const auto pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


    HittableList world;
    auto ground_sphere = std::make_shared<Sphere>(Point3(0, 0, -1), 0.5);
    auto fake_ground = std::make_shared<Sphere>(Point3(0, -100.5, -1), 100);

    world.add(ground_sphere);
    world.add(fake_ground);


#if USE_EASYX
    auto render = [&]{
        // 渲染部分
        for (int y = 0; y < image_height; ++y) {
            const auto height_vec = static_cast<float>(y) * pixel_delta_v;
            for (int x = 0; x < image_width; ++x) {
                auto pixel_center = pixel100_loc + (static_cast<float>(x) * pixel_delta_u) + height_vec;
                const auto Ray_direction = pixel_center - camera_center;
                Ray light(camera_center, Ray_direction);
                auto write_Color = ray_color(light, world);
                putpixel(x, y, write_Color.to_easyx());
            }
        }
    };

    initgraph(image_width, image_height);
    BeginBatchDraw();
    auto last_frame_time = std::chrono::steady_clock::now();
    while (true) {
        render();
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_time);
        last_frame_time = now;
        auto fps = 1000.0 / duration.count();
        outtextxy(0, 0, fmt::format("FPS: {:.2f}", fps).c_str());
        FlushBatchDraw();
        if (GetAsyncKeyState(VK_ESCAPE)) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EndBatchDraw();
    closegraph();
#else
    // render ppm
    std::ofstream ppm_file("render.ppm");
    ppm_file << fmt::format("P3\n{} {}\n255\n", image_width, image_height);
    for (int y = 0; y < image_height; ++y) {
        const auto height_vec = static_cast<float>(y) * pixel_delta_v;
        for (int x = 0; x < image_width; ++x) {
            auto pixel_center = pixel100_loc + (static_cast<float>(x) * pixel_delta_u) + height_vec;
            const auto Ray_direction = pixel_center - camera_center;
            Ray light(camera_center, Ray_direction);
            auto write_Color = ray_color(light, world);
            ppm_file << fmt::format("{} {} {}\n", write_Color.red<int>(), write_Color.green<int>(), write_Color.blue<int>());
        }
    }
    ppm_file.close();

    // render png
    FILE* png_file = fopen("render.png", "wb");
    unsigned char* png_data = nullptr;
    png_data = new unsigned char[image_width * image_height * 3];
    for (int y = 0; y < image_height; ++y) {
        const auto height_vec = static_cast<float>(y) * pixel_delta_v;
        for (int x = 0; x < image_width; ++x) {
            auto pixel_center = pixel100_loc + (static_cast<float>(x) * pixel_delta_u) + height_vec;
            const auto Ray_direction = pixel_center - camera_center;
            Ray light(camera_center, Ray_direction);
            auto write_Color = ray_color(light, world);
            auto index = (y * image_width + x) * 3;
            png_data[index] = write_Color.red<int>();
            png_data[index + 1] = write_Color.green<int>();
            png_data[index + 2] = write_Color.blue<int>();
        }
    }
    svpng(png_file, image_width, image_height, png_data, false);
    fclose(png_file);
    delete[] png_data;
#endif
    return 0;
}

