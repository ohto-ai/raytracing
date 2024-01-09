#pragma once
// color.hh

#ifndef _OHTOAI_COLOR_H_
#define _OHTOAI_COLOR_H_

#include "vector.hh"
#include "type_base.hh"
#include <cstdint>
#include <type_traits>

namespace ohtoai {
    namespace color {
        // RGBA
        template<typename T>
        class Color : public math::Vector<T, 4> {
        public:
            using value_type = T;
            using vector_type = math::Vector<value_type, 4>;
            Color() : Color(0, 0, 0, 0xff) {};
            Color(const Color&) = default;
            explicit Color(uint32_t rgba) : Color(((rgba >> 24) & 0xff, rgba >> 16) & 0xff, (rgba >> 8) & 0xff, rgba & 0xff) {}
            template<typename U>
            Color(U r, U g, U b, U a = {}) : vector_type(static_cast<value_type>(r), static_cast<value_type>(g), static_cast<value_type>(b), static_cast<value_type>(a)) {}

            template<typename U>
            Color(const math::Vector<U, 3>& v)
                : Color(static_cast<value_type>(v[0])
                    , static_cast<value_type>(v[1])
                    , static_cast<value_type>(v[2])) {}
            template<typename U>
            Color(const math::Vector<U, 4>& v)
                : Color(static_cast<value_type>(v[0])
                    , static_cast<value_type>(v[1])
                    , static_cast<value_type>(v[2])
                    , static_cast<value_type>(v[3])) {}
            Color& operator=(const Color&) = default;
            ~Color() = default;

            template <int r_index, int g_index, int b_index, int a_index
                    , uint8_t r_mask = 0xff, uint8_t g_mask = 0xff, uint8_t b_mask = 0xff, uint8_t a_mask = 0xff>
            uint32_t to_color() const {
                auto r = static_cast<int>(red());
                auto g = static_cast<int>(green());
                auto b = static_cast<int>(blue());
                auto a = static_cast<int>(alpha());
                if (r < 0)
                    r = 0;
                else if (r > 0xff)
                    r = 0xff;
                if (g < 0)
                    g = 0;
                else if (g > 0xff)
                    g = 0xff;
                if (b < 0)
                    b = 0;
                else if (b > 0xff)
                    b = 0xff;

                if (a < 0)
                    a = 0;
                else if (a > 0xff)
                    a = 0xff;
                return static_cast<uint32_t>(((r << r_index) & 0xff) | ((g << g_index) & 0xff) | ((b << b_index) & 0xff) | ((a << a_index) & 0xff));
            }

            uint32_t to_rgba() const {
                return to_color<24, 16, 8, 0>();
            }

            uint32_t to_rgb() const {
                return to_color<16, 8, 0, 24, 0xff, 0xff, 0xff, 0x00>();
            }

            uint32_t to_argb() const {
                return to_color<16, 8, 0, 24>();
            }

            uint32_t to_abgr() const {
                return to_color<0, 8, 16, 24>();
            }

            uint32_t to_bgr() const {
                return to_color<0, 8, 16, 24, 0xff, 0xff, 0xff, 0x00>();
            }

            uint32_t to_easyx_color() const {
                return to_bgr();
            }

            const value_type& red() const { return (*this)[0]; }
            const value_type& green() const { return (*this)[1]; }
            const value_type& blue() const { return (*this)[2]; }
            const value_type& alpha() const { return (*this)[3]; }
            value_type& red() { return (*this)[0]; }
            value_type& green() { return (*this)[1]; }
            value_type& blue() { return (*this)[2]; }
            value_type& alpha() { return (*this)[3]; }

            template<typename U>
            static Color from_rgb(U r, U g, U b) {
                return rgba(r, g, b, 0xff);
            }

            static Color from_rgb(uint32 rgb) {
                return from_rgba((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff, 0xff);
            }

            static Color from_rgba(uint32 rgba) {
                return rgba((rgb >> 24) & 0xff, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
            }

            template<typename U>
            static Color from_rgba(U r, U g, U b, U a) {
                return Color(r, g, b, a);
            }

        };

        enum {
            Black			= 0x000000ff,
            Blue			= 0x0000aaff,
            Green			= 0x00aa00ff,
            Cyan			= 0x00aaaaff,
            Red				= 0xaa0000ff,
            Magenta			= 0xaa00aaff,
            Brown			= 0xaa5500ff,
            LightGray		= 0xaaaaaaff,
            DarkGray		= 0x555555ff,
            LightBlue		= 0x5555ffff,
            LightGreen		= 0x55ff55ff,
            LightCyan		= 0x55ffffff,
            LightRed		= 0xff5555ff,
            LightMagenta	= 0xff55ffff,
            Yellow			= 0xffff55ff,
            White			= 0xffffffff,
        };
        using Colorf = Color<float>;
        using Colord = Color<double>;
        using Colori = Color<int>;
    }
}

#endif // !_OHTOAI_COLOR_H_
