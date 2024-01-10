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
        class Color : protected math::Vector<T, 4> {
        public:
            using vector_type = math::Vector<T, 4>;
            using typename vector_type::value_type;
            using vector_type::Dimension;

            constexpr Color() = default;
            constexpr Color(const Color&) = default;
            constexpr Color(Color&&) = default;
            constexpr Color& operator=(const Color&) = default;
            constexpr Color& operator=(Color &&) = default;
            ~Color() = default;

            constexpr Color(uint32_t rgba) : Color((rgba >> 24) & 0xff, (rgba >> 16) & 0xff, (rgba >> 8) & 0xff, rgba & 0xff) {}
            template<typename U>
            constexpr Color(U r, U g, U b, U a = {})
                : vector_type(static_cast<value_type>(r), static_cast<value_type>(g)
                , static_cast<value_type>(b), static_cast<value_type>(a)) {}

            template<typename U>
            constexpr Color(const math::Vector<U, 3>& v)
                : Color(static_cast<value_type>(v[0])
                    , static_cast<value_type>(v[1])
                    , static_cast<value_type>(v[2])) {}
            template<typename U>
            constexpr Color(const math::Vector<U, 4>& v)
                : Color(static_cast<value_type>(v[0])
                    , static_cast<value_type>(v[1])
                    , static_cast<value_type>(v[2])
                    , static_cast<value_type>(v[3])) {}

            Color& operator+=(const Color& v) {
                vector_type::operator+=(v);
                return *this;
            }

            Color& operator-=(const Color& v) {
                vector_type::operator-=(v);
                return *this;
            }

            Color& operator*=(const Color& v) {
                vector_type::operator*=(v);
                return *this;
            }

            Color& operator*=(const value_type& t) {
                vector_type::operator*=(t);
                return *this;
            }

            Color& operator/=(const value_type& t) {
                vector_type::operator/=(t);
                return *this;
            }

            constexpr vector_type& to_vector() const { return *this; }
            vector_type& to_vector() { return *this; }

            template <int r_index, int g_index, int b_index, int a_index
                    , uint8_t r_mask = 0xff, uint8_t g_mask = 0xff, uint8_t b_mask = 0xff, uint8_t a_mask = 0xff>
            constexpr uint32_t to_color() const {
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
                return static_cast<uint32_t>(((r & r_mask) << r_index ) | ((g & g_mask) << g_index) | ((b & b_mask) << b_index) | ((a & a_mask) << a_index));
            }

            constexpr uint32_t to_rgba() const {
                return to_color<24, 16, 8, 0>();
            }

            constexpr uint32_t to_rgb() const {
                return to_color<16, 8, 0, 24, 0xff, 0xff, 0xff, 0x00>();
            }

            constexpr uint32_t to_argb() const {
                return to_color<16, 8, 0, 24>();
            }

            constexpr uint32_t to_abgr() const {
                return to_color<0, 8, 16, 24>();
            }

            constexpr uint32_t to_bgr() const {
                return to_color<0, 8, 16, 24, 0xff, 0xff, 0xff, 0x00>();
            }

            constexpr uint32_t to_easyx_color() const {
                return to_bgr();
            }

            constexpr value_type red() const { return (*this)[0]; }
            constexpr value_type green() const { return (*this)[1]; }
            constexpr value_type blue() const { return (*this)[2]; }
            constexpr value_type alpha() const { return (*this)[3]; }
            value_type& red() { return (*this)[0]; }
            value_type& green() { return (*this)[1]; }
            value_type& blue() { return (*this)[2]; }
            value_type& alpha() { return (*this)[3]; }

            template<typename U = uint8_t>
            constexpr static Color rgba(U r, U g, U b, U a) {
                return Color(r, g, b, a);
            }

            constexpr static Color rgba(uint32_t rgba) {
                return Color(rgba);
            }

            template<typename U>
            constexpr static Color rgb(U r, U g, U b) {
                return rgba(r, g, b, 0xff);
            }

            constexpr static Color rgb(uint32_t rgb) {
                return rgba<uint8_t>((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff, 0xff);
            }

            template<typename C1, typename C2, typename = std::enable_if_t<std::conjunction_v<std::is_convertible<C1, Color>, std::is_convertible<C2, Color>>>>
            constexpr static Color mix(const C1& c1, const C2& c2, value_type t) {
                return Color(c1) * (1 - t) + Color(c2) * t;
            }

            template<typename C2>
            constexpr auto mix(const C2& c2, value_type t) const {
                return mix(*this, c2, t);
            }

            // inline constexpr static Color Black    { 0x000000ff};
            // inline constexpr static Color Blue			= 0x0000aaff;
            // inline constexpr static Color Green			= 0x00aa00ff;
            // inline constexpr static Color Cyan			= 0x00aaaaff;
            // inline constexpr static Color Red			    = 0xaa0000ff;
            // inline constexpr static Color Magenta		    = 0xaa00aaff;
            // inline constexpr static Color Brown			= 0xaa5500ff;
            // inline constexpr static Color LightGray		= 0xaaaaaaff;
            // inline constexpr static Color DarkGray		= 0x555555ff;
            // inline constexpr static Color LightBlue		= 0x5555ffff;
            // inline constexpr static Color LightGreen		= 0x55ff55ff;
            // inline constexpr static Color LightCyan		= 0x55ffffff;
            // inline constexpr static Color LightRed		= 0xff5555ff;
            // inline constexpr static Color LightMagenta	= 0xff55ffff;
            // inline constexpr static Color Yellow			= 0xffff55ff;
            // inline constexpr static Color White			= 0xffffffff;
        };

        template<typename T>
        constexpr Color<T> operator+(const Color<T>& v1, const Color<T>& v2) {
            return Color<T>::rgba(v1.red() + v2.red(), v1.green() + v2.green(), v1.blue() + v2.blue(), v1.alpha() + v2.alpha());
        }

        template<typename T>
        constexpr Color<T> operator-(const Color<T>& v1, const Color<T>& v2) {
            return Color<T>::rgba(v1.red() - v2.red(), v1.green() - v2.green(), v1.blue() - v2.blue(), v1.alpha() - v2.alpha());
        }

        template<typename T>
        constexpr Color<T> operator*(const Color<T>& v, const typename Color<T>::value_type& t) {
            return Color<T>::rgba(v.red() * t, v.green() * t, v.blue() * t, v.alpha() * t);
        }

        template<typename T>
        constexpr Color<T> operator*(const typename Color<T>::value_type& t, const Color<T>& v) {
            return v * t;
        }

        template<typename T>
        constexpr Color<T> operator/(const Color<T>& v, const typename Color<T>::value_type& t) {
            return Color<T>::rgba(v.red() / t, v.green() / t, v.blue() / t, v.alpha() / t);
        }

        template<typename T, typename U>
        constexpr bool operator==(const Color<T>& v1, const Color<U>& v2) {
            return Color<T>::vector_type::operator==(v1, v2);
        }

        template<typename T, typename U>
        constexpr bool operator!=(const Color<T>& v1, const Color<U>& v2) {
            return !(v1 == v2);
        }

        using Colorf = Color<float>;
        using Colord = Color<double>;
        using Colori = Color<int>;
    }
}

#endif // !_OHTOAI_COLOR_H_
