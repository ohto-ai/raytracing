#include "ray.hh"
#include "color.hh"
#include "point.hh"
#include <easyx.h>

using Ray = ohtoai::math::Rayd;
using Color = ohtoai::color::Colord;
using Point3 = ohtoai::math::Point3d;
using Vec3 = ohtoai::math::Vec3d;

double hit_sphere(const Point3& center, double radius, const Ray& light) {
    Vec3 origin = light.origin() - center;
    const auto a = light.direction().dot(light.direction());
    const auto b = 2.0 * origin.dot(light.direction());
    const auto c = origin.dot(origin) - radius * radius;
    const auto delta = b * b - 4 * a * c;
    if (delta < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(delta)) / (2.0 * a);
    }
}

Color ray_color(const Ray& Light) {
    const auto t = hit_sphere(Point3(0, 0, -1), 0.5, Light);
    // 计算法线
    if (t > 0) {
        Vec3 Normal = (Light.at(t) - Vec3(0, 0, -1)).normalized();
        return 255 * 0.5 * Color(Normal.x() + 1, Normal.y() + 1, Normal.z() + 1);
    }
    auto unit_direction = Light.direction().normalized();
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

    initgraph(image_width, image_height);
    BeginBatchDraw();
    // 渲染部分
    for (int y = 0; y < image_height; ++y) {
        const auto height_vec = static_cast<float>(y) * pixel_delta_v;
        for (int x = 0; x < image_width; ++x) {
            auto pixel_center = pixel100_loc + (static_cast<float>(x) * pixel_delta_u) + height_vec;
            const auto Ray_direction = pixel_center - camera_center;
            Ray light(camera_center, Ray_direction);
            auto write_Color = ray_color(light);
            putpixel(x, y, write_Color.to_easyx_color());
        }
    }
    EndBatchDraw();
    system("pause");
    return 0;
}

