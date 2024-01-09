#include "ray.hh"
#include "color.hh"
#include <easyx.h>

int main() {
    using namespace ohtoai::math;
    using namespace ohtoai::color;
    using namespace ohtoai;

    const auto AspectRatio = 16.0 / 9.0;
    const int ImageWidth = 640;
    const int ImageHeight = static_cast<int>(ImageWidth / AspectRatio);

    // camera
    const auto FocalLength = 1.0;
    const auto ViewportHeight = 2.0;
    const auto ViewportWidth = ViewportHeight * (static_cast<double>(ImageWidth) / ImageHeight);
    const auto CameraCenter = Point3f(0, 0, 0);

    // 计算横纵向量 u 和 v
    const auto ViewportU = Vec3f(ViewportWidth, 0, 0);
    const auto ViewportV = Vec3f(0, -ViewportHeight, 0);

    // 由像素间距计算横纵增量向量
    const auto PixelDeltaU = ViewportU / ImageWidth;
    const auto PixelDeltaV = ViewportV / ImageHeight;

    // 计算左上角像素的位置
    const auto ViewportUpperLeft = CameraCenter - Vec3f(0, 0, FocalLength) - ViewportU / 2 - ViewportV / 2;
    const auto Pixel100Loc = ViewportUpperLeft + 0.5 * (PixelDeltaU + PixelDeltaV);

    initgraph(ImageWidth, ImageHeight);
    BeginBatchDraw();
    // 渲染部分
    Colorf WriteColor;
    for (size_t Y = 0; Y < ImageHeight; ++Y) {
        const auto HeightVec = static_cast<float>(Y) * PixelDeltaV;
        for (size_t X = 0; X < ImageWidth; ++X) {
            auto PixelCenter = Pixel100Loc + (static_cast<float>(X) * PixelDeltaU) + HeightVec;
            const auto RayDirection = PixelCenter - CameraCenter;
            Rayf Light(CameraCenter, RayDirection);
            WriteColor = [](const Rayf& Light) {
                const auto UnitDirection = Light.direction().normalized();
                const auto t = 0.5f * (UnitDirection[1] + 1.0f);
                return (Colorf(color::White) * (1.0f - t) + Colorf::rgb(127, 178, 255) * t);
                }(Light);
            putpixel(X, Y, WriteColor.to_easyx_color());
        }
    }
    EndBatchDraw();
    system("pause");
    return 0;
}

