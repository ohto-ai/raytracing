#pragma once
// vector.hh

#ifndef _OHTOAI_VECTOR_H_
#define _OHTOAI_VECTOR_H_

#include "type_base.hh"
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <array>

namespace ohtoai {
    namespace math {
        template <size_t Dimension>
        class Vector: public std::array<real, Dimension> {
        public:
            using array_type = std::array<real, Dimension>;

            constexpr Vector() = default;
            constexpr Vector(const Vector&) = default;
            constexpr Vector(Vector&&) = default;
            constexpr Vector& operator=(const Vector&) = default;
            constexpr Vector& operator=(Vector &&) = default;
            ~Vector() = default;

            template<typename... Args>
            constexpr explicit Vector(Args ...args) : array_type {{static_cast<real>(args)...}} {
                static_assert(sizeof...(args) == Dimension, "Number of arguments must match the vector size");
            }

            Vector& operator+=(const Vector& v) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] += v[i];
                return *this;
            }

            Vector& operator-=(const Vector& v) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] -= v[i];
                return *this;
            }

            Vector& operator*=(const Vector& v) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] *= v[i];
                return *this;
            }

            Vector& operator*=(real t) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] *= t;
                return *this;
            }

            Vector& operator/=(real t) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] /= t;
                return *this;
            }

            Vector operator-() const {
                Vector neg;
                for (size_t i = 0; i < Dimension; ++i)
                    neg[i] = -(*this)[i];
                return neg;
            }

            auto length2() const {
                real sum = 0;
                for (size_t i = 0; i < Dimension; ++i)
                    sum += (*this)[i] * (*this)[i];
                return sum;
            }

            auto length() const {
                return std::sqrt(length2());
            }

            Vector& normalize() {
                return *this /= length();
            }

            Vector normalized() const {
                return *this / length();
            }

            constexpr bool operator==(const Vector& v) const {
                for (size_t i = 0; i < Dimension; ++i)
                    if ((*this)[i] != v[i])
                        return false;
                return true;
            }

            constexpr bool operator!=(const Vector& v) const {
                return !(*this == v);
            }

            constexpr real dot(const Vector& v) const {
                real sum = 0;
                for (size_t i = 0; i < Dimension; ++i)
                    sum += (*this)[i] * v[i];
                return sum;
            }

            constexpr auto cross(const Vector& v) const {
                static_assert(Dimension == 3, "Cross product is only defined for 3D vectors");
                return make_vector(
                    (*this)[1] * v[2] - (*this)[2] * v[1],
                    (*this)[2] * v[0] - (*this)[0] * v[2],
                    (*this)[0] * v[1] - (*this)[1] * v[0]);
            }

            constexpr size_t size() const { return Dimension; }

            real& x() { return (*this)[0]; }
            real& y() { return (*this)[1]; }
            real& z() { return (*this)[2]; }
            real& w() { return (*this)[3]; }

            const real& x() const { return (*this)[0]; }
            const real& y() const { return (*this)[1]; }
            const real& z() const { return (*this)[2]; }
            const real& w() const { return (*this)[3]; }
        };

        template <typename... Args>
        constexpr auto make_vector(Args... args) {
            return Vector<sizeof...(args)>(std::forward<Args>(args)...);
        }

        template <size_t Dimension>
        const auto make_random_vector() {
            Vector<Dimension> v;
            for (size_t i = 0; i < Dimension; ++i)
                v[i] = random_real();
            return v;
        }

        template <size_t Dimension>
        const auto make_random_vector(real min, real max) {
            Vector<T, Dimension> v;
            for (size_t i = 0; i < Dimension; ++i)
                v[i] = random_real(min, max);
            return v;
        }

        const auto generate_random_vector_in_sphere() {
            while (true) {
                auto v = make_random_vector<3>();
                if (v.length2() <= 1.0)
                    return v;
            }
        }

        const auto generate_random_vector_on_sphere() {
            return generate_random_vector_in_sphere().normalized();
        }

        const auto generate_random_vector_on_hemisphere(const Vector<3>& normal) {
            auto v = generate_random_vector_on_sphere();
            if (v.dot(normal) > 0)
                return v;
            else
                return -v;
        }

        template<size_t Dimension>
        constexpr Vector<Dimension> operator+(const Vector<Dimension>& v1, const Vector<Dimension>& v2) {
            Vector<Dimension> sum(v1);
            return sum += v2;
        }

        template<size_t Dimension>
        constexpr Vector<Dimension> operator-(const Vector<Dimension>& v1, const Vector<Dimension>& v2) {
            Vector<Dimension> diff(v1);
            return diff -= v2;
        }

        template<size_t Dimension>
        constexpr Vector<Dimension> operator*(const Vector<Dimension>& v, real t) {
            Vector<Dimension> prod(v);
            return prod *= t;
        }

        template<size_t Dimension>
        constexpr Vector<Dimension> operator*(real t, const Vector<Dimension>& v) {
            return v * t;
        }

        template<size_t Dimension>
        constexpr Vector<Dimension> operator/(const Vector<Dimension>& v, real t) {
            Vector<Dimension> quot(v);
            return quot /= t;
        }

        template<size_t Dimension>
        constexpr Vector<Dimension> operator*(const Vector<Dimension>& v1, const Vector<Dimension>& v2) {
            Vector<Dimension> prod(v1);
            return prod *= v2;
        }

        using Vec2 = Vector<2>;
        using Vec3 = Vector<3>;
        using Vec4 = Vector<4>;

        template <typename... Args>
        constexpr auto make_point(Args... args) {
            return make_vector(std::forward<Args>(args)...);
        }
        using Point2 = Vector<2>;
        using Point3 = Vector<3>;
    }
}

#endif // !_OHTOAI_VECTOR_H_
