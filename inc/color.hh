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
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0) : Color(static_cast<uint32_t>(r) | (g << 8) | (b << 16) | (a << 24) ) {}
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

        static Color rgb(uint8_t r, uint8_t g, uint8_t b) {
            return Color(r, g, b);
        }

        static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            return Color(r, g, b, a);
        }

        enum {
            Black			= 0x000000,
            Blue			= 0xaa0000,
            Green			= 0x00aa00,
            Cyan			= 0xaaaa00,
            Red				= 0x0000aa,
            Magenta			= 0xaa00aa,
            Brown			= 0x0055aa,
            LightGray		= 0xaaaaaa,
            DarkGray		= 0x555555,
            LightBlue		= 0xff5555,
            LightGreen		= 0x55ff55,
            LightCyan		= 0xffff55,
            LightRed		= 0x5555ff,
            LightMagenta	= 0xff55ff,
            Yellow			= 0x55ffff,
            White			= 0xffffff,
        };
        protected:
            union abgr_t{
                uint32_t rgba;
                struct {
                    uint8_t a;
                    uint8_t b;
                    uint8_t g;
                    uint8_t r;
                };
            };
            union rgba_t{
                uint32_t rgba;
                struct {
                    uint8_t r;
                    uint8_t g;
                    uint8_t b;
                    uint8_t a;
                };
            };
            typename std::conditional<type::endian::native == type::endian::big, abgr_t, rgba_t>::type c;
    };
}

#endif // !_OHTOAI_COLOR_H_
