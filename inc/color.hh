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
            Color() = default;
            Color(const Color&) = default;
            Color(uint32_t rgba) : Color(rgba & 0xff, (rgba >> 8) & 0xff, (rgba >> 16) & 0xff, (rgba >> 24) & 0xff) {}
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

            explicit operator uint32_t() const {
                return to_uint32();
            }

            uint32_t to_uint32() const {
                uint32_t r = red() > 255 ? 255 : red() < 0 ? 0 : static_cast<uint32_t>(red());
                uint32_t g = green() > 255 ? 255 : green() < 0 ? 0 : static_cast<uint32_t>(green());
                uint32_t b = blue() > 255 ? 255 : blue() < 0 ? 0 : static_cast<uint32_t>(blue());
                uint32_t a = alpha() > 255 ? 255 : alpha() < 0 ? 0 : static_cast<uint32_t>(alpha());
                return (a << 24) | (b << 16) | (g << 8) | r;
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
            static Color rgb(U r, U g, U b) {
                return rgba(r, g, b, {});
            }

            template<typename U>
            static Color rgba(U r, U g, U b, U a) {
                return Color(r, g, b, a);
            }

        };

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
        using Colorf = Color<float>;
        using Colord = Color<double>;
        using Colori = Color<int>;
    }
}

#endif // !_OHTOAI_COLOR_H_
