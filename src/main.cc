#include "camera.hh"
#include "hittable.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include <fmt/format.h>
#include <chrono>
#include <thread>
#ifdef _WIN32
#define EXPORT_EASYX
#endif
#define EXPORT_PNG
#define EXPORT_PPM


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
    ohtoai::math::Camera camera;
    ohtoai::math::HittableList world;
    auto ground_sphere = std::make_shared<ohtoai::math::Sphere>(ohtoai::math::make_point(0, 0, -1), 0.5);
    auto fake_ground = std::make_shared<ohtoai::math::Sphere>(ohtoai::math::make_point(0, -100.5, -1), 100);

    world.add(ground_sphere);
    world.add(fake_ground);

    camera.aspect_ratio = 16.0 / 9.0;
    camera.image_width = 640;
    camera.image_height = 360;
    camera.samples_per_pixel = 100;
    camera.max_depth = 50;

#ifdef EXPORT_EASYX
    initgraph(camera.image_width, camera.image_height);
    BeginBatchDraw();

    while(true) {
        if (GetAsyncKeyState(VK_ESCAPE)) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        camera.render(world, [&](int x, int y, const auto& color) {
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
#endif // EXPORT_EASYX
    return 0;
}

