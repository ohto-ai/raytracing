#pragma once
// device.hh

#ifndef _OHTOAI_DEVICE_H_
#define _OHTOAI_DEVICE_H_

#include "color.hh"
#include "point.hh"
#include <easyx.h>

namespace ohtoai{
    class Device {
    public:
        Device(int width, int height) : width(width), height(height) {
            initgraph(width, height);
        }

        ~Device() {
            closegraph();
        }

        void clear() {
            cleardevice();
        }

        void setPixel(int x, int y, Color color) {
            putpixel(x, y, color);
        }

        void setLineColor(Color color) {
            setlinecolor(color);
        }

        void drawLine(int x1, int y1, int x2, int y2) {
            line(x1, y1, x2, y2);
        }

        void drawLine(const ohtoai::math::Point2i& p1, const ohtoai::math::Point2i& p2) {
            line(p1.x(), p1.y(), p2.x(), p2.y());
        }

    protected:
        int width;
        int height;
    };
}

#endif // !_OHTOAI_DEVICE_H_
