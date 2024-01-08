#pragma once
// vector.hh

#ifndef _OHTOAI_VECTOR_H_
#define _OHTOAI_VECTOR_H_

#include <type_traits>
#include <algorithm>

namespace ohtoai {
    namespace math {
        template <typename T, size_t D>
        class Vector {
        public:
            Vector() = default;
            Vector(const Vector&) = default;
            template<typename... Args>
            Vector(Args ...args) {
                static_assert(sizeof...(args) == D, "Number of arguments must match the vector size");
                initializeElements(args...);
            }
            Vector& operator=(const Vector&) = default;
            ~Vector() = default;

            T& operator[](size_t i) { return e[i]; }
            const T& operator[](size_t i) const { return e[i]; }

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

            template<typename U>
            Vector& operator*=(const U& t) {
                for (size_t i = 0; i < D; ++i)
                    e[i] *= t;
                return *this;
            }

            template<typename U>
            Vector& operator/=(const U& t) {
                for (size_t i = 0; i < D; ++i)
                    e[i] /= t;
                return *this;
            }

            T length_squared() const {
                return std::reduce(std::begin(e), std::end(e), std::multiplies());
            }

            T length() const {
                return std::sqrt(length_squared());
            }

            Vector& normalize() {
                return *this /= length();
            }

            Vector normalized() const {
                return *this / length();
            }

            template<typename U>
            typename std::common_type<T, U>::type dot(const Vector<U, D>& v) const {
                return std::transform_reduce(std::begin(e), std::end(e), std::begin(v.e), std::common_type<T, U>::type{});
            }

            template<typename U>
            Vector<typename std::common_type<T, U>::type, D> cross(const Vector<U, D>& v) const {
                static_assert(D == 3, "Cross product is only defined for 3D vectors");
                return Vector<typename std::common_type<T, U>::type, D>(e[1] * v[2] - e[2] * v[1],
                    e[2] * v[0] - e[0] * v[2],
                    e[0] * v[1] - e[1] * v[0]);
            }

            constexpr size_t size() const { return D; }

        protected:
            T e[D]{};

            template<typename Arg, typename... Args>
            void initializeElements(Arg first, Args... rest) {
                e[D - sizeof...(Args) - 1] = static_cast<T>(first);
                if constexpr (sizeof...(Args) > 0)
                    initializeElements(rest...);
            }
        };

        template <typename Arg, typename... Args>
        auto createVector(Arg arg, Args... args) {
            return Vector<Arg, sizeof...(args) + 1>(arg, args...);
        }

        template <typename T, size_t D>
        const Vector<T, D>& operator-(const Vector<T, D>& v) {
            auto _v(v)
            std::transform(std::begin(_v.e), std::end(_v.e), std::begin(_v.e), std::negate());
            return _v;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type<T, U>::type, D> operator+(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type<T, U>::type, D> sum;
            std::transform(std::begin(v1.e), std::end(v1.e), std::begin(v2.e), std::begin(sum.e), std::plus());
            return sum;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type<T, U>::type, D> operator-(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type<T, U>::type, D> diff;
            std::transform(std::begin(v1.e), std::end(v1.e), std::begin(v2.e), std::begin(diff.e), std::minus());
            return diff;
        }

        template<typename T, size_t D>
        Vector<T, D> operator*(const Vector<T, D>& v, const T& t) {
            Vector<T, D> prod(v);
            return prod *= t;
        }

        template<typename T, size_t D>
        Vector<T, D> operator/(const Vector<T, D>& v, const T& t) {
            Vector<T, D> quot(v);
            return quot /= t;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type<T, U>::type, D> operator*(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type<T, U>::type, D> prod;
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

        using Vec2f = Vector<float, 2>;
        using Vec3f = Vector<float, 3>;
    }
}

#endif // !_OHTOAI_VECTOR_H_
