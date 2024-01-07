#pragma once
// vector.hh

#ifndef _OHTOAI_VECTOR_H_
#define _OHTOAI_VECTOR_H_

#include <type_traits>

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

            Vector& operator+=(const Vector& v) {
                for (size_t i = 0; i < D; ++i)
                    e[i] += v[i];
                return *this;
            }

            Vector& operator-=(const Vector& v) {
                for (size_t i = 0; i < D; ++i)
                    e[i] -= v[i];
                return *this;
            }

            Vector& operator*=(const Vector& v) {
                for (size_t i = 0; i < D; ++i)
                    e[i] *= v[i];
                return *this;
            }

            Vector& operator*=(const T& t) {
                for (size_t i = 0; i < D; ++i)
                    e[i] *= t;
                return *this;
            }

            Vector& operator/=(const T& t) {
                for (size_t i = 0; i < D; ++i)
                    e[i] /= t;
                return *this;
            }

            T length_squared() const {
                T sum = 0;
                for (size_t i = 0; i < D; ++i)
                    sum += e[i] * e[i];
                return sum;
            }

            T length() const {
                return sqrt(length_squared());
            }

            Vector& normalize() {
                return *this /= length();
            }

            Vector normalized() const {
                return *this / length();
            }

            T dot(const Vector& v) const {
                T sum = 0;
                for (size_t i = 0; i < D; ++i)
                    sum += e[i] * v[i];
                return sum;
            }

            Vector cross(const Vector& v) const {
                static_assert(D == 3, "Cross product is only defined for 3D vectors");
                return Vector(e[1] * v[2] - e[2] * v[1],
                    e[2] * v[0] - e[0] * v[2],
                    e[0] * v[1] - e[1] * v[0]);
            }

            size_t size() const { return D; }

        protected:
            T e[D]{};

            template<typename Arg, typename... Args>
            void initializeElements(Arg first, Args... rest) {
                e[D - sizeof...(Args) - 1] = static_cast<T>(first);
                initializeElements(rest...);
            }
            void initializeElements() {}
        };

        template <typename Arg, typename... Args>
        auto createVector(Arg arg, Args... args) {
            return Vector<Arg, sizeof...(args) + 1>(arg, args...);
        }

        template <typename T, size_t D>
        const Vector<T, D>& operator-(const Vector<T, D>& v) {
            auto _v(v)
            for (size_t i = 0; i < D; ++i)
                _v[i] = -_v[i];
            return _v;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type<T, U>::type, D> operator+(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type<T, U>::type, D> sum;
            for (size_t i = 0; i < D; ++i)
                sum[i] = v1[i] + v2[i];
            return sum;
        }

        template<typename T, size_t D, typename U>
        Vector<typename std::common_type<T, U>::type, D> operator-(const Vector<T, D>& v1, const Vector<U, D>& v2) {
            Vector<typename std::common_type<T, U>::type, D> diff;
            for (size_t i = 0; i < D; ++i)
                diff[i] = v1[i] - v2[i];
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
            for (size_t i = 0; i < D; ++i)
                prod[i] = v1[i] * v2[i];
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
