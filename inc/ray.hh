#pragma once
// ray.hh

#ifndef _OHTOAI_RAY_H_
#define _OHTOAI_RAY_H_

#include "vector.hh"

namespace ohtoai {
    namespace math {
        template <typename T>
        class Ray {
        public:
            Ray() = default;
            Ray(const Ray&) = default;
            Ray(const Point3<T>& origin, const Vector<T, 3>& direction) : o(origin), d(direction) {}
            Ray& operator=(const Ray&) = default;
            ~Ray() = default;

            Point3<T> operator()(T t) const {
                return o + t * d;
            }

            Point3<T> origin() const { return o; }
            Vector<T, 3> direction() const { return d; }
            Point3<T> at(T t) const { return o + t * d; }

        private:
            Point3<T> o;
            Vector<T, 3> d;
        };

        using Rayf = Ray<float>;
        using Rayd = Ray<double>;
        using Rayi = Ray<int>;
    }
}

#endif // !_OHTOAI_RAY_H_
