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
        template <typename T, size_t D>
        class Vector: public std::array<T, D> {
        public:
            using array_type = std::array<T, D>;
            using typename array_type::value_type;
            constexpr static size_t Dimension = D;

            constexpr Vector() = default;
            constexpr Vector(const Vector&) = default;
            constexpr Vector(Vector&&) = default;
            constexpr Vector& operator=(const Vector&) = default;
            constexpr Vector& operator=(Vector &&) = default;
            ~Vector() = default;

            template<typename... Args>
            constexpr explicit Vector(Args ...args) : array_type {{static_cast<T>(args)...}} {
                static_assert(sizeof...(args) == D, "Number of arguments must match the vector size");
            }

            template<typename U>
            Vector& operator+=(const Vector<U, Dimension>& v) {
                std::transform(array_type::begin(), array_type::end(), v.begin(), array_type::begin(), std::plus());
                return *this;
            }

            template<typename U>
            Vector& operator-=(const Vector<U, Dimension>& v) {
                std::transform(array_type::begin(), array_type::end(), v.begin(), array_type::begin(), std::minus());
                return *this;
            }

            template<typename U>
            Vector& operator*=(const Vector<U, Dimension>& v) {
                std::transform(array_type::begin(), array_type::end(), v.begin(), array_type::begin(), std::multiplies());
                return *this;
            }

            Vector& operator*=(const value_type& t) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] *= t;
                return *this;
            }

            Vector& operator/=(const value_type& t) {
                for (size_t i = 0; i < Dimension; ++i)
                    (*this)[i] /= t;
                return *this;
            }

            const Vector& operator-() const {
                static_assert(std::is_signed_v<value_type>, "Vector must be signed type");
                Vector neg;
                std::transform(array_type::begin(), array_type::end(), neg.begin(), std::negate());
                return *this;
            }

            auto length2() const {
                return std::reduce(array_type::begin(), array_type::end(), value_type{}, [](const value_type& a, const value_type& b) { return a + b * b; });
            }

            auto length() const {
                return std::sqrt(length2());
            }

            Vector& normalize() {
                static_assert(std::is_floating_point_v<value_type>, "Vector must be floating point type");
                return *this /= length();
            }

            Vector normalized() const {
                static_assert(std::is_floating_point_v<value_type>, "Vector must be floating point type");
                return *this / length();
            }

            template<typename U>
            typename std::common_type_t<T, U> dot(const Vector<U, Dimension>& v) const {
                return std::transform_reduce(array_type::begin(), array_type::end(), v.begin(), std::common_type_t<T, U>{});
            }

            template<typename U>
            auto cross(const Vector<U, Dimension>& v) const {
                static_assert(Dimension == 3, "Cross product is only defined for 3D vectors");
                return Vector(
                    (*this)[1] * v[2] - (*this)[2] * v[1],
                    (*this)[2] * v[0] - (*this)[0] * v[2],
                    (*this)[0] * v[1] - (*this)[1] * v[0]);
            }

            constexpr size_t size() const { return Dimension; }

            value_type& x() { return (*this)[0]; }
            value_type& y() { return (*this)[1]; }
            value_type& z() { return (*this)[2]; }
            value_type& w() { return (*this)[3]; }

            const value_type& x() const { return (*this)[0]; }
            const value_type& y() const { return (*this)[1]; }
            const value_type& z() const { return (*this)[2]; }
            const value_type& w() const { return (*this)[3]; }

        };

        template <typename Arg, typename... Args>
        constexpr auto createVector(Arg arg, Args... args) {
            return Vector<Arg, sizeof...(args) + 1>(arg, args...);
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type_t<T, U>, D> operator+(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type_t<T, U>, D> sum;
            std::transform(v1.begin(), v1.end(), v2.begin(), sum.begin(), std::plus());
            return sum;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type_t<T, U>, D> operator-(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type_t<T, U>, D> diff;
            std::transform(v1.begin(), v1.end(), v2.begin(), diff.begin(), std::minus());
            return diff;
        }

        template<typename T, size_t D>
        Vector<T, D> operator*(const Vector<T, D>& v, const typename Vector<T, D>::value_type& t) {
            Vector<T, D> prod(v);
            return prod *= t;
        }

        template<typename T, size_t D>
        Vector<T, D> operator*(const typename Vector<T, D>::value_type& t, const Vector<T, D>& v) {
            return v * t;
        }

        template<typename T, size_t D>
        Vector<T, D> operator/(const Vector<T, D>& v, const typename Vector<T, D>::value_type& t) {
            Vector<T, D> quot(v);
            return quot /= t;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type_t<T, U>, D> operator*(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type_t<T, U>, D> prod;
            std::transform(v1.begin(), v1.end(), v2.begin(), prod.begin(), std::multiplies());
            return prod;
        }

        template<typename T, size_t D, typename U>
        bool operator==(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            for (size_t i = 0; i < D; ++i)
                if (v1[i] != v2[i])
                    return false;
            return true;
        }

        template<typename T, size_t D, typename U>
        bool operator!=(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            return !(v1 == v2);
        }

        template <typename T>
        using Vec2 = Vector<T, 2>;
        template <typename T>
        using Vec3 = Vector<T, 3>;

        using Vec2f = Vec2<float>;
        using Vec3f = Vec3<float>;
        using Vec2d = Vec2<double>;
        using Vec3d = Vec3<double>;
        using Vec2i = Vec2<int>;
        using Vec3i = Vec3<int>;

        template <typename T>
        using Point2 = Vector<T, 2>;
        template <typename T>
        using Point3 = Vector<T, 3>;

        using Point2f = Point2<float>;
        using Point3f = Point3<float>;
        using Point2d = Point2<double>;
        using Point3d = Point3<double>;
        using Point2i = Point2<int>;
        using Point3i = Point3<int>;
    }
}

#endif // !_OHTOAI_VECTOR_H_
