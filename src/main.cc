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
    using ohtoai::math::Sphere;
    using ohtoai::math::Color;

    auto MaterialGround = std::make_shared<ohtoai::math::Lambertian>(Color(0.8, 0.8, 0.0));
    auto MaterialCenter = std::make_shared<ohtoai::math::Lambertian>(Color(0.7, 0.3, 0.3));
    auto MaterialLeft = std::make_shared<ohtoai::math::Metal>(Color(0.8, 0.8, 0.8), 0.3);
    auto MaterialRight = std::make_shared<ohtoai::math::Metal>(Color(0.8, 0.6, 0.2), 1.0);

    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(0.0, -100.5, -1.0), 100.0, MaterialGround));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(0.0, 0.0, -1.0), 0.5, MaterialCenter));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(-1.0, 0.0, -1.0), 0.5, MaterialLeft));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(1.0, 0.0, -1.0), 0.5, MaterialRight));

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
    saveimage("render.bmp");
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

