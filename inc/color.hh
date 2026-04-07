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
            OHTOAI_HOST_DEVICE Color() = default;
            OHTOAI_HOST_DEVICE Color(const Color&) = default;
            OHTOAI_HOST_DEVICE Color(Color&&) = default;
            OHTOAI_HOST_DEVICE Color& operator=(const Color&) = default;
            OHTOAI_HOST_DEVICE Color& operator=(Color &&) = default;
            OHTOAI_HOST_DEVICE ~Color() = default;

            OHTOAI_HOST_DEVICE Color(uint32_t rgba) : Color((rgba >> 24) & 0xff, (rgba >> 16) & 0xff, (rgba >> 8) & 0xff, rgba & 0xff) {}
            OHTOAI_HOST_DEVICE Color(real r, real g, real b, real a = {})
                : Vec4(r, g ,b ,a) {}

            OHTOAI_HOST_DEVICE Color(const Vec3& v)
                : Color(v[0], v[1], v[2]) {}
            OHTOAI_HOST_DEVICE Color(const Vec4& v)
                : Color(v[0], v[1], v[2], v[3]) {}

            OHTOAI_HOST_DEVICE Color& operator+=(const Color& v) {
                Vec4::operator+=(v);
                return *this;
            }

            OHTOAI_HOST_DEVICE Color& operator-=(const Color& v) {
                Vec4::operator-=(v);
                return *this;
            }

            OHTOAI_HOST_DEVICE Color& operator*=(const Color& v) {
                Vec4::operator*=(v);
                return *this;
            }

            OHTOAI_HOST_DEVICE Color& operator*=(real t) {
                Vec4::operator*=(t);
                return *this;
            }

            OHTOAI_HOST_DEVICE Color& operator/=(real t) {
                Vec4::operator/=(t);
                return *this;
            }

            OHTOAI_HOST_DEVICE Color gamma_correction() const {
                return Color(liner_to_gamma(red() / 255.0) * 255, liner_to_gamma(green() / 255.0) * 255, liner_to_gamma(blue()/255.0) * 255, alpha());
            }

            using Vec4::operator[];
            using Vec4::operator==;
            using Vec4::operator!=;

            OHTOAI_HOST_DEVICE Vec4 to_vector() const { return make_vector((*this)[0], (*this)[1], (*this)[2], (*this)[3]); }
            OHTOAI_HOST_DEVICE Vec4& to_vector() { return *this; }

            template <int r_index, int g_index, int b_index, int a_index
                    , uint8_t r_mask = 0xff, uint8_t g_mask = 0xff, uint8_t b_mask = 0xff, uint8_t a_mask = 0xff>
            OHTOAI_HOST_DEVICE uint32_t to_color() const {
                auto r = red<int>() < 0 ? 0 : (red<int>() > 255 ? 255 : red<int>());
                auto g = green<int>() < 0 ? 0 : (green<int>() > 255 ? 255 : green<int>());
                auto b = blue<int>() < 0 ? 0 : (blue<int>() > 255 ? 255 : blue<int>());
                auto a = alpha<int>() < 0 ? 0 : (alpha<int>() > 255 ? 255 : alpha<int>());
                return static_cast<uint32_t>(((r & r_mask) << r_index ) | ((g & g_mask) << g_index) | ((b & b_mask) << b_index) | ((a & a_mask) << a_index));
            }

            OHTOAI_HOST_DEVICE Color clamp(real max = 255.0) const {
                auto clamp_val = [](real v, real lo, real hi) -> real {
                    return v < lo ? lo : (v > hi ? hi : v);
                };
                return Color(clamp_val(red(), 0.0, max),
                             clamp_val(green(), 0.0, max),
                             clamp_val(blue(), 0.0, max),
                             clamp_val(alpha(), 0.0, max));
            }

            OHTOAI_HOST_DEVICE Color to_unit() const {
                return Color(red() / 255.0, green() / 255.0, blue() / 255.0, alpha());
            }

            OHTOAI_HOST_DEVICE Color to_ununit() const {
                return Color(red() * 255.0, green() * 255.0, blue() * 255.0, alpha());
            }

            OHTOAI_HOST_DEVICE uint32_t to_rgba() const {
                return to_color<24, 16, 8, 0>();
            }

            OHTOAI_HOST_DEVICE uint32_t to_rgb() const {
                return to_color<16, 8, 0, 24, 0xff, 0xff, 0xff, 0x00>();
            }

            OHTOAI_HOST_DEVICE uint32_t to_argb() const {
                return to_color<16, 8, 0, 24>();
            }

            OHTOAI_HOST_DEVICE uint32_t to_abgr() const {
                return to_color<0, 8, 16, 24>();
            }

            OHTOAI_HOST_DEVICE uint32_t to_bgr() const {
                return to_color<0, 8, 16, 24, 0xff, 0xff, 0xff, 0x00>();
            }

            OHTOAI_HOST_DEVICE uint32_t to_easyx_color() const {
                return to_bgr();
            }

            OHTOAI_HOST_DEVICE real red()      const { return (*this)[0]; }
            OHTOAI_HOST_DEVICE real green()    const { return (*this)[1]; }
            OHTOAI_HOST_DEVICE real blue()     const { return (*this)[2]; }
            OHTOAI_HOST_DEVICE real alpha()    const { return (*this)[3]; }

            template <typename U> OHTOAI_HOST_DEVICE U red()     const { return static_cast<U>(red()); }
            template <typename U> OHTOAI_HOST_DEVICE U green()   const { return static_cast<U>(green()); }
            template <typename U> OHTOAI_HOST_DEVICE U blue()    const { return static_cast<U>(blue()); }
            template <typename U> OHTOAI_HOST_DEVICE U alpha()   const { return static_cast<U>(alpha()); }

            OHTOAI_HOST_DEVICE real& red()   { return (*this)[0]; }
            OHTOAI_HOST_DEVICE real& green() { return (*this)[1]; }
            OHTOAI_HOST_DEVICE real& blue()  { return (*this)[2]; }
            OHTOAI_HOST_DEVICE real& alpha() { return (*this)[3]; }

            template<typename U = uint8_t>
            OHTOAI_HOST_DEVICE static Color rgba(U r, U g, U b, U a) {
                return Color(r, g, b, a);
            }

            OHTOAI_HOST_DEVICE static Color rgba(uint32_t rgba) {
                return Color(rgba);
            }

            template<typename U>
            OHTOAI_HOST_DEVICE static Color rgb(U r, U g, U b) {
                return rgba(r, g, b, {});
            }

            OHTOAI_HOST_DEVICE static Color rgb(uint32_t rgb) {
                return rgba<uint8_t>((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff, 0xff);
            }

            OHTOAI_HOST_DEVICE static Color mix(const Color& c1, const Color& c2, double t = 0.5) {
                return Color(c1) * (1 - t) + Color(c2) * t;
            }

            OHTOAI_HOST_DEVICE auto mix(const Color& c2, double t = 0.5) const {
                return mix(*this, c2, t);
            }
        };

        OHTOAI_HOST_DEVICE inline Color operator+(const Color& v1, const Color& v2) {
            return Color::rgba(v1.red() + v2.red(), v1.green() + v2.green(), v1.blue() + v2.blue(), v1.alpha() + v2.alpha());
        }

        OHTOAI_HOST_DEVICE inline Color operator-(const Color& v1, const Color& v2) {
            return Color::rgba(v1.red() - v2.red(), v1.green() - v2.green(), v1.blue() - v2.blue(), v1.alpha() - v2.alpha());
        }

        OHTOAI_HOST_DEVICE inline Color operator*(const Color& v1, const Color& v2) {
            return Color::rgba(v1.red() * v2.red(), v1.green() * v2.green(), v1.blue() * v2.blue(), v1.alpha() * v2.alpha());
        }


        OHTOAI_HOST_DEVICE inline Color operator*(const Color& v, real t) {
            return Color::rgba(v.red() * t, v.green() * t, v.blue() * t, v.alpha() * t);
        }

        OHTOAI_HOST_DEVICE inline Color operator*(real t, const Color& v) {
            return v * t;
        }

        OHTOAI_HOST_DEVICE inline Color operator/(const Color& v, real t) {
            return Color::rgba(v.red() / t, v.green() / t, v.blue() / t, v.alpha() / t);
        }

        OHTOAI_HOST_DEVICE inline Color make_color(uint32_t rgba) {
            return Color::rgba(rgba);
        }

        OHTOAI_HOST_DEVICE inline Color make_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff) {
            return Color::rgba(r, g, b, a);
        }
    }
}

#endif // !_OHTOAI_COLOR_H_
