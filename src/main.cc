#include "camera.hh"
#include "color.hh"
#include "cuda_scene.hh"
#include "render.hh"
#include <fmt/format.h>
#include <chrono>
#include <vector>
#include <cstdio>
#include <fstream>

#include <svpng/svpng.inc>

int main() {
    // -----------------------------------------------------------------------
    // Camera
    // -----------------------------------------------------------------------
    ohtoai::math::Camera camera;
    camera.aspect_ratio      = 16.0 / 9.0;
    camera.image_width       = 1280;
    camera.image_height      = 720;
    camera.samples_per_pixel = 500;
    camera.max_depth         = 50;
    camera.v_fov             = 20;
    camera.look_from         = ohtoai::math::make_point(13, 2, 3);
    camera.look_at           = ohtoai::math::make_point(0, 0, 0);
    camera.view_up           = ohtoai::math::make_vector(0, 1, 0);
    camera.defocus_angle     = 0.6;
    camera.focus_distance    = 10.0;
    camera.setup();

    // -----------------------------------------------------------------------
    // Scene (flat POD arrays — passed directly to the CUDA kernel)
    // -----------------------------------------------------------------------
    using ohtoai::math::Color;
    using ohtoai::math::CudaMaterial;
    using ohtoai::math::CudaSphere;
    using ohtoai::math::CudaScene;

    std::vector<CudaMaterial> materials;
    std::vector<CudaSphere>   spheres;

    // Helper lambdas to add scene objects
    auto add_lambertian = [&](Color albedo) -> int {
        int idx = (int)materials.size();
        materials.push_back({ CudaMaterial::Type::Lambertian, albedo, 0.0, 1.0 });
        return idx;
    };
    auto add_metal = [&](Color albedo, double fuzz) -> int {
        int idx = (int)materials.size();
        materials.push_back({ CudaMaterial::Type::Metal, albedo, fuzz, 1.0 });
        return idx;
    };
    auto add_dielectric = [&](double ir) -> int {
        int idx = (int)materials.size();
        materials.push_back({ CudaMaterial::Type::Dielectric, Color{}, 0.0, ir });
        return idx;
    };
    auto add_sphere = [&](double x, double y, double z, double r, int mat) {
        spheres.push_back({ ohtoai::math::make_point(x, y, z), r, mat });
    };

    // Ground
    int ground_mat = add_lambertian(Color::rgb(0x808080).to_unit());
    add_sphere(0, -1000, 0, 1000, ground_mat);

    // Three feature spheres
    int glass_mat  = add_dielectric(1.5);
    int lamb_mat   = add_lambertian(Color::rgb(0x1A3380).to_unit());
    int metal_mat  = add_metal(Color::rgb(0xCC9933).to_unit(), 0.0);
    add_sphere( 0, 1, 0, 1.0, glass_mat);
    add_sphere(-4, 1, 0, 1.0, lamb_mat);
    add_sphere( 4, 1, 0, 1.0, metal_mat);

    CudaScene scene {
        spheres.data(),   (int)spheres.size(),
        materials.data(), (int)materials.size()
    };

    // -----------------------------------------------------------------------
    // Render
    // -----------------------------------------------------------------------
    auto cam_params = camera.to_cuda_params();
    const int pixels = cam_params.image_width * cam_params.image_height;
    std::vector<uint8_t> output(pixels * 4, 0);

    {
        auto start = std::chrono::high_resolution_clock::now();
        ohtoai::math::render_cuda(cam_params, scene, output.data());
        auto end      = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        fmt::print("CUDA rendering time: {}ms  ({}×{}, {} spp)\n",
                   ms, cam_params.image_width, cam_params.image_height,
                   cam_params.samples_per_pixel);
    }

    // -----------------------------------------------------------------------
    // Save PNG
    // -----------------------------------------------------------------------
    {
        FILE* fp = fopen("render.png", "wb");
        if (fp) {
            svpng(fp, (unsigned)cam_params.image_width, (unsigned)cam_params.image_height,
                  output.data(), 1 /* RGBA */);
            fclose(fp);
            fmt::print("Saved render.png\n");
        }
    }

    // -----------------------------------------------------------------------
    // Save PPM
    // -----------------------------------------------------------------------
    {
        std::ofstream ofs("render.ppm");
        ofs << "P3\n" << cam_params.image_width << " " << cam_params.image_height << "\n255\n";
        for (int i = 0; i < pixels; ++i) {
            ofs << (int)output[i * 4 + 0] << " "
                << (int)output[i * 4 + 1] << " "
                << (int)output[i * 4 + 2] << "\n";
        }
        fmt::print("Saved render.ppm\n");
    }

    return 0;
}
