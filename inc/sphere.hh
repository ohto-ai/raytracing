#pragma once
// sphere.hh

#ifndef _OHTOAI_SPHERE_H_
#define _OHTOAI_SPHERE_H_

#include "hittable.hh"

namespace ohtoai{
    namespace math {
        template<typename T>
        class Sphere : public Hittable<T> {
        public:
            using value_type = T;
            using point_type = Point3<T>;
            using vector_type = Vec3<T>;
            using ray_type = Ray<T>;
            using hit_record_type = HitRecord<T>;

            Sphere() = default;
            Sphere(const point_type& center, value_type radius) : center_(center), radius_(radius) {}

            virtual bool hit(const ray_type& light, value_type ray_min, value_type ray_max, hit_record_type& rec) const override {
                vector_type origin = light.origin() - center_;
                const auto a = light.direction().dot(light.direction());
                const auto b = 2.0 * origin.dot(light.direction());
                const auto c = origin.dot(origin) - radius_ * radius_;
                const auto delta = b * b - 4 * a * c;
                if (delta < 0) {
                    return false;
                } else {
                    const auto t = (-b - sqrt(delta)) / (2.0 * a);
                    if (t < ray_min || t > ray_max) {
                        return false;
                    }
                    rec.t = t;
                    rec.point = light.at(t);
                    rec.set_face_normal(light, (rec.point - center_) / radius_);
                    return true;
                }
            }

        private:
            point_type center_;
            value_type radius_;
        };
    }
}

#endif // !_OHTOAI_SPHERE_H_
