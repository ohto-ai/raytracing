#pragma once
// sphere.hh

#ifndef _OHTOAI_SPHERE_H_
#define _OHTOAI_SPHERE_H_

#include "hittable.hh"

namespace ohtoai{
    namespace math {
        class Sphere : public Hittable {
        public:
            Sphere() = default;
            Sphere(const Point3& center, real radius) : center_(center), radius_(radius) {}

            virtual bool hit(const Ray& light, const Interval& ray_range, HitRecord& rec) const override {
                Vec3 origin = light.origin() - center_;
                const auto a = light.direction().dot(light.direction());
                const auto b = 2.0 * origin.dot(light.direction());
                const auto c = origin.dot(origin) - radius_ * radius_;
                const auto delta = b * b - 4 * a * c;
                if (delta < 0) {
                    return false;
                } else {
                    auto t = (-b - sqrt(delta)) / (2.0 * a);
                    if (!ray_range.surrounds(t)) {
                        t = (-b + sqrt(delta)) / (2.0 * a);
                        if (!ray_range.surrounds(t)) {
                            return false;
                        }
                    }
                    rec.t = t;
                    rec.point = light.at(t);
                    rec.set_face_normal(light, (rec.point - center_) / radius_);
                    return true;
                }
            }

        private:
            Point3 center_;
            real radius_;
        };
    }
}

#endif // !_OHTOAI_SPHERE_H_
