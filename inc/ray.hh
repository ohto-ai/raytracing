#pragma once
// ray.hh

#ifndef _OHTOAI_RAY_H_
#define _OHTOAI_RAY_H_

#include "vector.hh"

namespace ohtoai {
    namespace math {
        class Ray {
        public:
            constexpr Ray() = default;
            constexpr Ray(const Ray&) = default;
            constexpr Ray(Ray&&) = default;
            constexpr Ray& operator=(const Ray&) = default;
            constexpr Ray& operator=(Ray &&) = default;
            ~Ray() = default;

            constexpr Ray(const Point3& origin, const Vec3& direction) : o(origin), d(direction) {}

            constexpr Point3 operator()(real t) const {
                return o + t * d;
            }

            constexpr Point3 origin() const { return o; }
            constexpr Vec3 direction() const { return d; }
            constexpr Point3 at(real t) const { return o + t * d; }

        private:
            Point3 o;
            Vec3 d;
        };
    }
}

#endif // !_OHTOAI_RAY_H_
