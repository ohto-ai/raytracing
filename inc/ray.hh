#pragma once
// ray.hh

#ifndef _OHTOAI_RAY_H_
#define _OHTOAI_RAY_H_

#include "vector.hh"

namespace ohtoai {
    namespace math {
        class Ray {
        public:
            Ray() = default;
            Ray(const Ray&) = default;
            Ray(Ray&&) = default;
            Ray& operator=(const Ray&) = default;
            Ray& operator=(Ray &&) = default;
            ~Ray() = default;

            Ray(const Point3& origin, const Vec3& direction) : o(origin), d(direction) {}

            Point3 operator()(real t) const {
                return o + t * d;
            }

            Point3 origin() const { return o; }
            Vec3 direction() const { return d; }
            Point3 at(real t) const { return o + t * d; }

        private:
            Point3 o;
            Vec3 d;
        };
    }
}

#endif // !_OHTOAI_RAY_H_
