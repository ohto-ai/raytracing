#include "camera.hh"
#include "hittable.hh"
#include "color.hh"
#include "hittable_list.hh"
#include "sphere.hh"
#include "render.hh"
#include <fmt/format.h>
#include <chrono>
#include <thread>
#include <vector>

#ifdef _WIN32
#define EXPORT_EASYX
#endif
#define EXPORT_PNG
#define EXPORT_PPM

#if defined(EXPORT_EASYX)
#include <easyx.h>
#endif

#if defined(EXPORT_PNG)
#include <cstdio>
#include <svpng/svpng.inc>
#endif
#if defined(EXPORT_PPM)
#include <fstream>
#endif

int main() {
    ohtoai::math::Camera camera;

    camera.aspect_ratio      = 16.0 / 9.0;
    const int ratio          = 4;
    camera.image_width       = 640 / ratio;
    camera.image_height      = 360 / ratio;
    camera.samples_per_pixel = 100;
    camera.max_depth         = 50;
    camera.v_fov             = 90;
    camera.look_from         = ohtoai::math::make_point(-2, 2, 1);
    camera.look_at           = ohtoai::math::make_point(0, 0, -1);
    camera.view_up           = ohtoai::math::make_vector(0, 1, 0);

    // Initialise camera geometry once up-front so both rendering paths use
    // the same pre-computed values.
    camera.setup();

    using ohtoai::math::Color;

#ifdef EXPORT_EASYX
    // ---------------------------------------------------------------
    // Windows / EasyX path: render live into an EasyX window using the
    // polymorphic Camera::render_ray_tracing() pipeline.
    // ---------------------------------------------------------------
    using ohtoai::math::Sphere;
    ohtoai::math::HittableList world;
    auto MaterialGround = std::make_shared<ohtoai::math::Lambertian>(Color::rgb(0xCCCC00).to_unit());
    auto MaterialCenter = std::make_shared<ohtoai::math::Lambertian>(Color::rgb(0x1A3380).to_unit());
    auto MaterialLeft   = std::make_shared<ohtoai::math::Dielectric>(1.5);
    auto MaterialRight  = std::make_shared<ohtoai::math::Metal>(Color::rgb(0xCC9933).to_unit(), 0);

    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(0.0,  -100.5, -1.0), 100.0, MaterialGround));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(0.0,   0.0,   -1.0),   0.5, MaterialCenter));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(-1.0,  0.0,   -1.0),   0.5, MaterialLeft));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(-1.0,  0.0,   -1.0),  -0.4, MaterialLeft));
    world.add(std::make_shared<Sphere>(ohtoai::math::make_point(1.0,   0.0,   -1.0),   0.5, MaterialRight));

    initgraph(camera.image_width * ratio, camera.image_height * ratio);
    BeginBatchDraw();
    {
        auto start = std::chrono::high_resolution_clock::now();
        camera.render_ray_tracing(world, [&](int x, int y, const auto& color) {
            auto easyx_color = color.to_easyx_color();
            for (int i = 0; i < ratio; ++i)
                for (int j = 0; j < ratio; ++j)
                    putpixel(x * ratio + i, y * ratio + j, easyx_color);
            FlushBatchDraw();
        });
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        fmt::print("Use {} thread rendering time: {}ms\n", std::thread::hardware_concurrency(), duration);
    }
    EndBatchDraw();
    saveimage("render.bmp");
    // Wait for ESC to close the window
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    closegraph();
#else
    // ---------------------------------------------------------------
    // Non-EasyX path: render using render_cpu (or render_cuda when
    // OHTOAI_ENABLE_CUDA is defined) and save the result to disk.
    // ---------------------------------------------------------------
    using ohtoai::math::CudaMaterial;
    using ohtoai::math::CudaSphere;
    using ohtoai::math::CudaScene;

    // Scene as flat POD arrays — the same data used by both CPU and CUDA kernels.
    std::vector<CudaMaterial> materials = {
        { CudaMaterial::Type::Lambertian, Color::rgb(0xCCCC00).to_unit(), 0.0, 1.0 },  // 0: ground
        { CudaMaterial::Type::Lambertian, Color::rgb(0x1A3380).to_unit(), 0.0, 1.0 },  // 1: center
        { CudaMaterial::Type::Dielectric, Color{},                        0.0, 1.5 },  // 2: left (glass)
        { CudaMaterial::Type::Metal,      Color::rgb(0xCC9933).to_unit(), 0.0, 1.0 },  // 3: right
    };

    std::vector<CudaSphere> spheres = {
        { ohtoai::math::make_point(0.0,  -100.5, -1.0), 100.0, 0 },
        { ohtoai::math::make_point(0.0,   0.0,   -1.0),   0.5, 1 },
        { ohtoai::math::make_point(-1.0,  0.0,   -1.0),   0.5, 2 },
        { ohtoai::math::make_point(-1.0,  0.0,   -1.0),  -0.4, 2 },
        { ohtoai::math::make_point(1.0,   0.0,   -1.0),   0.5, 3 },
    };

    CudaScene scene {
        spheres.data(),   (int)spheres.size(),
        materials.data(), (int)materials.size()
    };

    auto cam_params  = camera.to_cuda_params();
    const int pixels = cam_params.image_width * cam_params.image_height;
    std::vector<uint8_t> output(pixels * 4, 0);

    {
        auto start = std::chrono::high_resolution_clock::now();
#ifdef OHTOAI_ENABLE_CUDA
        ohtoai::math::render_cuda(cam_params, scene, output.data());
        const char* backend = "CUDA";
#else
        ohtoai::math::render_cpu(cam_params, scene, output.data());
        const char* backend = "CPU";
#endif
        auto end      = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        fmt::print("{} rendering time: {}ms\n", backend, duration);
    }

#if defined(EXPORT_PNG)
    {
        FILE* fp = fopen("render.png", "wb");
        if (fp) {
            svpng(fp, (unsigned)cam_params.image_width, (unsigned)cam_params.image_height,
                  output.data(), 1 /* RGBA */);
            fclose(fp);
            fmt::print("Saved render.png\n");
        }
    }
#endif
#if defined(EXPORT_PPM)
    {
        std::ofstream ofs("render.ppm");
        ofs << "P3\n" << cam_params.image_width << " " << cam_params.image_height << "\n255\n";
        for (int i = 0; i < pixels; ++i)
            ofs << (int)output[i * 4 + 0] << " "
                << (int)output[i * 4 + 1] << " "
                << (int)output[i * 4 + 2] << "\n";
        fmt::print("Saved render.ppm\n");
    }
#endif
#endif // EXPORT_EASYX

    return 0;
}
