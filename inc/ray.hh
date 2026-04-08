#pragma once
// ray.hh

#ifndef _OHTOAI_RAY_H_
#define _OHTOAI_RAY_H_

#include "vector.hh"

namespace ohtoai {
    namespace math {
        class Ray {
        public:
            OHTOAI_HOST_DEVICE Ray() = default;
            OHTOAI_HOST_DEVICE Ray(const Ray&) = default;
            OHTOAI_HOST_DEVICE Ray(Ray&&) = default;
            OHTOAI_HOST_DEVICE Ray& operator=(const Ray&) = default;
            OHTOAI_HOST_DEVICE Ray& operator=(Ray &&) = default;
            OHTOAI_HOST_DEVICE ~Ray() = default;

            OHTOAI_HOST_DEVICE Ray(const Point3& origin, const Vec3& direction) : o(origin), d(direction) {}

            OHTOAI_HOST_DEVICE Point3 operator()(real t) const {
                return o + t * d;
            }

            OHTOAI_HOST_DEVICE Point3 origin() const { return o; }
            OHTOAI_HOST_DEVICE Vec3 direction() const { return d; }
            OHTOAI_HOST_DEVICE Point3 at(real t) const { return o + t * d; }

        private:
            Point3 o;
            Vec3 d;
        };

        OHTOAI_HOST_DEVICE inline Ray make_ray(const Point3& origin, const Vec3& direction) {
            return Ray(origin, direction);
        }
    }
}

#endif // !_OHTOAI_RAY_H_
