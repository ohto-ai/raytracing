#pragma once
// vector.hh

#ifndef _OHTOAI_VECTOR_H_
#define _OHTOAI_VECTOR_H_

#include <type_traits>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace ohtoai {
    namespace math {
        template <typename T, size_t D>
        class Vector {
        public:
            using value_type = T;
            Vector() = default;
            Vector(const Vector&) = default;
            template<typename... Args>
            explicit Vector(Args ...args) {
                static_assert(sizeof...(args) == D, "Number of arguments must match the vector size");
                initializeElements(args...);
            }
            Vector& operator=(const Vector&) = default;
            ~Vector() = default;

            template<typename U>
            Vector& assign(const Vector<U, D>& v) {
                if constexpr (std::is_same_v<value_type, U>) {
                    return operator=(v);
                } else {
                    return operator=(cast<value_type>(v));
                }
            }

            value_type& operator[](size_t i) { return e[i]; }
            const value_type& operator[](size_t i) const { return e[i]; }

            template<typename U>
            Vector& operator+=(const Vector<U, D>& v) {
                std::transform(std::begin(e), std::end(e), std::begin(v.e), std::begin(e), std::plus());
                return *this;
            }

            template<typename U>
            Vector& operator-=(const Vector<U, D>& v) {
                std::transform(std::begin(e), std::end(e), std::begin(v.e), std::begin(e), std::minus());
                return *this;
            }

            template<typename U>
            Vector& operator*=(const Vector<U, D>& v) {
                std::transform(std::begin(e), std::end(e), std::begin(v.e), std::begin(e), std::multiplies());
                return *this;
            }

            Vector& operator*=(const value_type& t) {
                for (size_t i = 0; i < D; ++i)
                    e[i] *= t;
                return *this;
            }

            Vector& operator/=(const value_type& t) {
                for (size_t i = 0; i < D; ++i)
                    e[i] /= t;
                return *this;
            }

            auto length2() const {
                return std::reduce(std::begin(e), std::end(e), value_type{}, [](const value_type& a, const value_type& b) { return a + b * b; });
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
            typename std::common_type_t<T, U> dot(const Vector<U, D>& v) const {
                return std::transform_reduce(std::begin(e), std::end(e), std::begin(v.e), std::common_type_t<T, U>{});
            }

            template<typename U>
            auto cross(const Vector<U, D>& v) const {
                static_assert(D == 3, "Cross product is only defined for 3D vectors");
                return createVector(
                    e[1] * v.e[2] - e[2] * v.e[1],
                    e[2] * v.e[0] - e[0] * v.e[2],
                    e[0] * v.e[1] - e[1] * v.e[0]);
            }

            constexpr size_t size() const { return D; }

            auto begin() { return std::begin(e); }
            auto end() { return std::end(e); }
            const auto begin() const { return std::begin(e); }
            const auto end() const { return std::end(e); }

            value_type& x() { return e[0]; }
            value_type& y() { return e[1]; }
            value_type& z() { return e[2]; }
            value_type& w() { return e[3]; }

            const value_type& x() const { return e[0]; }
            const value_type& y() const { return e[1]; }
            const value_type& z() const { return e[2]; }
            const value_type& w() const { return e[3]; }
        protected:
            value_type e[D]{};

            template<typename Arg, typename... Args>
            void initializeElements(Arg first, Args... rest) {
                e[D - sizeof...(Args) - 1] = static_cast<value_type>(first);
                if constexpr (sizeof...(Args) > 0)
                    initializeElements(rest...);
            }

            template <typename Arg, typename... Args>
            friend auto createVector(Arg arg, Args... args);

            template <typename T, size_t D>
            friend const Vector<T, D>& operator-(const Vector<T, D>& v);

            template<typename T, size_t D, typename U>
            friend Vector<typename std::common_type_t<T, U>, D> operator+(const Vector<T, D>& v1, const Vector<U, D>& v2);

            template<typename T, size_t D, typename U>
            friend Vector<typename std::common_type_t<T, U>, D> operator-(const Vector<T, D>& v1, const Vector<U, D>& v2);

            template<typename T, size_t D>
            friend Vector<T, D> operator*(const Vector<T, D>& v, const typename Vector<T, D>::value_type& t);

            template<typename T, size_t D>
            friend Vector<T, D> operator*(const typename Vector<T, D>::value_type& t, const Vector<T, D>& v);

            template<typename T, size_t D>
            friend Vector<T, D> operator/(const Vector<T, D>& v, const typename Vector<T, D>::value_type& t);

            template<typename T, size_t D, typename U>
            friend Vector<typename std::common_type_t<T, U>, D> operator*(const Vector<T, D>& v1, const Vector<U, D>& v2);

            template<typename T, size_t D, typename U>
            friend bool operator==(const Vector<T, D>& v1, const Vector<U, D>& v2);

            template<typename T, size_t D, typename U>
            friend bool operator!=(const Vector<T, D>& v1, const Vector<U, D>& v2);

            template<typename T, size_t D, typename U>
            friend Vector<U, D> cast(const Vector<T, D>& v);
        };

        template <typename Arg, typename... Args>
        auto createVector(Arg arg, Args... args) {
            return Vector<Arg, sizeof...(args) + 1>(arg, args...);
        }

        template <typename T, size_t D>
        const Vector<T, D>& operator-(const Vector<T, D>& v) {
            auto _v(v)
            std::transform(std::begin(_v), std::end(_v), std::begin(_v), std::negate());
            return _v;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type_t<T, U>, D> operator+(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type_t<T, U>, D> sum;
            std::transform(std::begin(v1), std::end(v1), std::begin(v2), std::begin(sum), std::plus());
            return sum;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type_t<T, U>, D> operator-(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type_t<T, U>, D> diff;
            std::transform(std::begin(v1), std::end(v1), std::begin(v2), std::begin(diff), std::minus());
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
            std::transform(std::begin(v1.e), std::end(v1.e), std::begin(v2.e), std::begin(prod.e), std::multiplies());
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

        template<typename T, size_t D, typename U>
        Vector<U, D> cast(const Vector<T, D>& v) {
            Vector<U, D> _v;
            std::transform(std::begin(v), std::end(v), std::begin(_v), [](const T& t) { return static_cast<U>(t); });
            return _v;
        }

        using Vec2f = Vector<float, 2>;
        using Vec3f = Vector<float, 3>;
    }
}

#endif // !_OHTOAI_VECTOR_H_
