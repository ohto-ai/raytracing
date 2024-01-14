#pragma once
// color.hh

#ifndef _OHTOAI_COLOR_H_
#define _OHTOAI_COLOR_H_

#include "vector.hh"
#include "type_base.hh"
#include <cstdint>
#include <type_traits>

namespace ohtoai {
    namespace math {
        // RGBA
        class Color : protected Vec4 {
        public:
            constexpr Color() = default;
            constexpr Color(const Color&) = default;
            constexpr Color(Color&&) = default;
            constexpr Color& operator=(const Color&) = default;
            constexpr Color& operator=(Color &&) = default;
            ~Color() = default;

            constexpr Color(uint32_t rgba) : Color((rgba >> 24) & 0xff, (rgba >> 16) & 0xff, (rgba >> 8) & 0xff, rgba & 0xff) {}
            constexpr Color(real r, real g, real b, real a = {})
                : Vec4(r, g ,b ,a) {}

            constexpr Color(const Vec3& v)
                : Color(v[0], v[1], v[2]) {}
            constexpr Color(const Vec4& v)
                : Color(v[0], v[1], v[2], v[3]) {}

            Color& operator+=(const Color& v) {
                Vec4::operator+=(v);
                return *this;
            }

            Color& operator-=(const Color& v) {
                Vec4::operator-=(v);
                return *this;
            }

            Color& operator*=(const Color& v) {
                Vec4::operator*=(v);
                return *this;
            }

            Color& operator*=(real t) {
                Vec4::operator*=(t);
                return *this;
            }

            Color& operator/=(real t) {
                Vec4::operator/=(t);
                return *this;
            }

            using Vec4::operator[];
            using Vec4::operator==;
            using Vec4::operator!=;

            constexpr Vec4& to_vector() const { return make_vector((*this)[0], (*this)[1], (*this)[2], (*this)[3]); }
            Vec4& to_vector() { return *this; }

            template <int r_index, int g_index, int b_index, int a_index
                    , uint8_t r_mask = 0xff, uint8_t g_mask = 0xff, uint8_t b_mask = 0xff, uint8_t a_mask = 0xff>
            constexpr uint32_t to_color() const {
                auto r = std::clamp(red<int>(), 0, 255);
                auto g = std::clamp(green<int>(), 0, 255);
                auto b = std::clamp(blue<int>(), 0, 255);
                auto a = std::clamp(alpha<int>(), 0, 255);
                return static_cast<uint32_t>(((r & r_mask) << r_index ) | ((g & g_mask) << g_index) | ((b & b_mask) << b_index) | ((a & a_mask) << a_index));
            }

            constexpr Color clamp(real max = 255.0) const {
                return Color(std::clamp(red(), 0.0, max),
                             std::clamp(green(), 0.0, max),
                             std::clamp(blue(), 0.0, max),
                             std::clamp(alpha(), 0.0, max));
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

            constexpr real red()      const { return (*this)[0]; }
            constexpr real green()    const { return (*this)[1]; }
            constexpr real blue()     const { return (*this)[2]; }
            constexpr real alpha()    const { return (*this)[3]; }

            template <typename U> constexpr U red()     const { return static_cast<U>(red()); }
            template <typename U> constexpr U green()   const { return static_cast<U>(green()); }
            template <typename U> constexpr U blue()    const { return static_cast<U>(blue()); }
            template <typename U> constexpr U alpha()   const { return static_cast<U>(alpha()); }

            real& red()   { return (*this)[0]; }
            real& green() { return (*this)[1]; }
            real& blue()  { return (*this)[2]; }
            real& alpha() { return (*this)[3]; }

            template<typename U = uint8_t>
            constexpr static Color rgba(U r, U g, U b, U a) {
                return Color(r, g, b, a);
            }

            constexpr static Color rgba(uint32_t rgba) {
                return Color(rgba);
            }

            template<typename U>
            constexpr static Color rgb(U r, U g, U b) {
                return rgba(r, g, b, {});
            }

            constexpr static Color rgb(uint32_t rgb) {
                return rgba<uint8_t>((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff, 0xff);
            }

            constexpr static Color mix(const Color& c1, const Color& c2, double t = 0.5) {
                return Color(c1) * (1 - t) + Color(c2) * t;
            }

            constexpr auto mix(const Color& c2, double t = 0.5) const {
                return mix(*this, c2, t);
            }
        };

        constexpr Color operator+(const Color& v1, const Color& v2) {
            return Color::rgba(v1.red() + v2.red(), v1.green() + v2.green(), v1.blue() + v2.blue(), v1.alpha() + v2.alpha());
        }

        constexpr Color operator-(const Color& v1, const Color& v2) {
            return Color::rgba(v1.red() - v2.red(), v1.green() - v2.green(), v1.blue() - v2.blue(), v1.alpha() - v2.alpha());
        }

        constexpr Color operator*(const Color& v, real t) {
            return Color::rgba(v.red() * t, v.green() * t, v.blue() * t, v.alpha() * t);
        }

        constexpr Color operator*(real t, const Color& v) {
            return v * t;
        }

        constexpr Color operator/(const Color& v, real t) {
            return Color::rgba(v.red() / t, v.green() / t, v.blue() / t, v.alpha() / t);
        }

        constexpr Color make_color(uint32_t rgba) {
            return Color::rgba(rgba);
        }

        constexpr Color make_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff) {
            return Color::rgba(r, g, b, a);
        }
    }
}

#endif // !_OHTOAI_COLOR_H_
