#pragma once
// color.hh

#ifndef _OHTOAI_COLOR_H_
#define _OHTOAI_COLOR_H_

#include "vector.hh"
#include <cstdint>

namespace ohtoai {
    // RGBA
    class Color {
    public:
        Color() = default;
        Color(const Color&) = default;
        Color(uint32_t rgba) {
            c.rgba = rgba;
        }
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0) : Color(static_cast<uint32_t>(r) | (g << 8) | (b << 16) | (a << 18) ) {}
        template<typename T>
        Color(const math::Vector<T, 3>& v)
            : Color(static_cast<uint8_t>(v[0])
                , static_cast<uint8_t>(v[1])
                , static_cast<uint8_t>(v[2])) {}
        template<typename T>
        Color(const math::Vector<T, 4>& v)
            : Color(static_cast<uint8_t>(v[0])
                , static_cast<uint8_t>(v[1])
                , static_cast<uint8_t>(v[2])
                , static_cast<uint8_t>(v[3])) {}
        Color& operator=(const Color&) = default;
        ~Color() = default;
        operator uint32_t() const {
            return c.rgba;
        }
        operator uint32_t&() {
            return c.rgba;
        }

        const uint8_t& red() const { return c.r; }
        const uint8_t& green() const { return c.g; }
        const uint8_t& blue() const { return c.b; }
        const uint8_t& alpha() const { return c.a; }
        uint8_t& red() { return c.r; }
        uint8_t& green() { return c.g; }
        uint8_t& blue() { return c.b; }
        uint8_t& alpha() { return c.a; }

        enum {
            Black			= 0x000000,
            Blue			= 0xAA0000,
            Green			= 0x00AA00,
            Cyan			= 0xAAAA00,
            Red				= 0x0000AA,
            Magenta			= 0xAA00AA,
            Brown			= 0x0055AA,
            LightGray		= 0xAAAAAA,
            DarkGray		= 0x555555,
            LightBlue		= 0xFF5555,
            LightGreen		= 0x55FF55,
            LightCyan		= 0xFFFF55,
            LightRed		= 0x5555FF,
            LightMagenta	= 0xFF55FF,
            Yellow			= 0x55FFFF,
            White			= 0xFFFFFF,
        };
        protected:
            union {
                uint32_t rgba;
                struct {
#ifdef __BIG_ENDIAN__
                    uint8_t a;
                    uint8_t b;
                    uint8_t g;
                    uint8_t r;
#else
                    uint8_t r;
                    uint8_t g;
                    uint8_t b;
                    uint8_t a;
#endif
                };
            } c;
    };
}

#endif // !_OHTOAI_COLOR_H_
