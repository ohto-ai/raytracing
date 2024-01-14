#include "camera.hh"
#include "hittable.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include <fmt/format.h>
#include <format>
#include <chrono>
#include <thread>
#define EXPORT_PPM
#define EXPORT_PNG
#define EXPORT_EASYX


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
    camera.max_depth = 50;

    initgraph(camera.image_width, camera.image_height);
    BeginBatchDraw();

    {
        auto start = std::chrono::high_resolution_clock::now();
        camera.render_mt(world, [&](int x, int y, const auto& color) {
            putpixel(x, y, color.to_easyx_color());
            FlushBatchDraw();
        });
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        fmt::print("Use {} thread rendering time: {}ms\n", std::thread::hardware_concurrency(), duration);
    }

    EndBatchDraw();
    while(true) {
        if (GetAsyncKeyState(VK_ESCAPE)) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    closegraph();
    return 0;
}

