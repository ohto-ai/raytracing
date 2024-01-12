#pragma once
// hittable.hh

#ifndef _OHTOAI_HITTABLE_H_
#define _OHTOAI_HITTABLE_H_

#include "vector.hh"
#include "ray.hh"
#include "interval.hh"

namespace ohtoai{
    namespace math {
        template<typename T>
        class HitRecord {
        public:
            using value_type = T;
            using point_type = Point3<T>;
            using vector_type = Vec3<T>;


            void set_face_normal(const Ray<T>& light, const Vec3<T>& outward_normal) {
                front_face = light.direction().dot(outward_normal) < 0;
                normal = front_face ? outward_normal : -outward_normal;
            }

        public:
            Point3<T> point;
            Vec3<T> normal;
            T t;
            bool front_face;
        };

        template<typename T>
        class Hittable {
        public:
            using value_type = T;
            using point_type = Point3<T>;
            using vector_type = Vec3<T>;
            using hit_record_type = HitRecord<T>;
            using ray_type = Ray<T>;
            using interval_type = Interval<T>;

            virtual bool hit(const ray_type& light, const interval_type& ray_range , hit_record_type& rec) const = 0;
        };
    }
}

#endif // !_OHTOAI_HITTABLE_H_
