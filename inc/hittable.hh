#pragma once
// hittable.hh

#ifndef _OHTOAI_HITTABLE_H_
#define _OHTOAI_HITTABLE_H_

#include "vector.hh"
#include "ray.hh"
#include "interval.hh"

namespace ohtoai{
    namespace math {
        class Material;
        class HitRecord {
        public:
            void set_face_normal(const Ray& light, const Vec3& outward_normal) {
                front_face = light.direction().dot(outward_normal) < 0;
                normal = front_face ? outward_normal : -outward_normal;
            }

        public:
            Point3 point;
            Vec3 normal;
            real t;
            bool front_face;
            std::shared_ptr<Material> material;
        };

        class Hittable {
        public:
            virtual bool hit(const Ray& light, const Interval& ray_range , HitRecord& rec) const = 0;
        };
    }
}

#endif // !_OHTOAI_HITTABLE_H_
