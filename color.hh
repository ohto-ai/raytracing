#pragma once
// color.hh

#ifndef _OHTOAI_COLOR_H_
#define _OHTOAI_COLOR_H_

#include "vector.hh"
#include "type_base.hh"
#include <cstdint>
#include <type_traits>

namespace ohtoai {
    // RGBA
    class Color {
    public:
        Color() = default;
        Color(const Color&) = default;
        Color(uint32_t rgba) {
            c.rgba = rgba;
        }
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0) : Color(static_cast<uint32_t>(a) | (b << 8) | (g << 16) | (r << 18) ) {}
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
            Blue			= 0x0000AA,
            Green			= 0x00AA00,
            Cyan			= 0x00AAAA,
            Red				= 0xAA0000,
            Magenta			= 0xAA00AA,
            Brown			= 0xAA5500,
            LightGray		= 0xAAAAAA,
            DarkGray		= 0x555555,
            LightBlue		= 0x5555FF,
            LightGreen		= 0x55FF55,
            LightCyan		= 0x55FFFF,
            LightRed		= 0xFF5555,
            LightMagenta	= 0xFF55FF,
            Yellow			= 0xFFFF55,
            White			= 0xFFFFFF,
        };
        protected:
            union {
                uint32_t rgba;
                std::conditional<type::endian::native == type::endian::little, 
                    struct { uint8_t a,b,g,r; },
                    struct { uint8_t r,g,b,a; }
                    >;
            } c;
    };
}

#endif // !_OHTOAI_COLOR_H_
