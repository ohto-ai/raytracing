#include "ray.hh"
#include "device.hh"
#include <cassert>

int main() {
    using namespace ohtoai::math;
    using namespace ohtoai;
    Device dev(800, 600);

    auto v1 = createVector(1, 2, 3);
    auto v2 = createVector(4.0, 5.0, 6.0);

    assert(v1 + v2 == Vec3f(5, 7, 9));
    assert(v1 - v2 == Vec3f(-3, -3, -3));

    dev.clear();
    dev.setLineColor(Color::Red);
    dev.drawLine(50, 100, 100, 100);
    dev.setLineColor(Color::Green);
    dev.drawLine(50, 200, 100, 200);
    dev.setLineColor(Color::Blue);
    dev.drawLine(50, 300, 100, 300);
    dev.setLineColor(Color::Yellow);
    dev.drawLine(50, 400, 100, 400);

    system("pause");
    return 0;
}

