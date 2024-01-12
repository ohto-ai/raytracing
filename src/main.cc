#include "camera.hh"
#include "hittable.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include <spdlog/fmt/fmt.h>
#include <chrono>
#include <thread>
// #define EXPORT_PPM
#define EXPORT_PNG
// #define EXPORT_EASYX


#if defined(EXPORT_EASYX)
#include <easyx.h>
#endif

#if defined(EXPORT_PNG)
#include <svpng/svpng.inc>
#endif
#if defined(EXPORT_PPM)
#include <fstream>
#endif

int main() {
    ohtoai::math::Camera<double> camera;
    ohtoai::math::HittableList<double> world;
    auto ground_sphere = std::make_shared<ohtoai::math::Sphere<double>>(ohtoai::math::Point3d(0, 0, -1), 0.5);
    auto fake_ground = std::make_shared<ohtoai::math::Sphere<double>>(ohtoai::math::Point3d(0, -100.5, -1), 100);

    world.add(ground_sphere);
    world.add(fake_ground);

    camera.aspect_ratio = 16.0 / 9.0;
    camera.image_width = 640;
    camera.image_height = 360;
    camera.samples_per_pixel = 100;
#if defined(EX_SHOWCONSOLE) && defined(EXPORT_EASYX)
    initgraph(camera.image_width, camera.image_height);
    BeginBatchDraw();

    camera.render(world, [&](int x, int y, const auto& color) {
        putpixel(x, y, color.to_easyx_color());
    });

    EndBatchDraw();
    while(true) {
        if (GetAsyncKeyState(VK_ESCAPE)) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    closegraph();
#endif
#if defined(EXPORT_PPM)
    // render ppm
    std::ofstream ppm_file("render.ppm");
    ppm_file << fmt::format("P3\n{} {}\n255\n", camera.image_width, camera.image_height);
    camera.render(world, [&](int x, int y, const auto& color) {
        ppm_file << fmt::format("{} {} {}\n", color.red<int>(), color.green<int>(), color.blue<int>());
    });
    ppm_file.close();
#endif
#if defined(EXPORT_PNG)
    // render png
    FILE* png_file = fopen("render.png", "wb");
    auto png_data = new unsigned char[camera.image_width * camera.image_height * 3];
    if (png_data != nullptr) {
        camera.render(world, [&](int x, int y, const auto& color) {
            auto index = (y * camera.image_width + x) * 3;
            png_data[index] = color.red<int>();
            png_data[index + 1] = color.green<int>();
            png_data[index + 2] = color.blue<int>();
        });
        svpng(png_file, camera.image_width, camera.image_height, png_data, false);
        fclose(png_file);
        delete[] png_data;
    }
#endif
    return 0;
}

