#pragma once
// hittable_list.hh

#ifndef _OHTOAI_HITTABLE_LIST_H_
#define _OHTOAI_HITTABLE_LIST_H_

#include "hittable.hh"

namespace ohtoai{
    namespace math {
        template<typename T>
        class HittableList : public Hittable<T> {
        public:
            using value_type = T;
            using point_type = Point3<T>;
            using vector_type = Vec3<T>;
            using ray_type = Ray<T>;
            using hit_record_type = HitRecord<T>;
            using hittable_type = Hittable<T>;

            HittableList() = default;
            explicit HittableList(const std::shared_ptr<hittable_type>& object) { add(object); }

            void clear() { objects_.clear(); }
            void add(const std::shared_ptr<hittable_type>& object) { objects_.push_back(object); }

            virtual bool hit(const ray_type& light, value_type ray_min, value_type ray_max, hit_record_type& rec) const override {
                hit_record_type temp_rec;
                bool hit_anything = false;
                auto closest_so_far = ray_max;

                for (const auto& object : objects_) {
                    if (object->hit(light, ray_min, closest_so_far, temp_rec)) {
                        hit_anything = true;
                        closest_so_far = temp_rec.t;
                        rec = temp_rec;
                    }
                }

                return hit_anything;
            }

        private:
            std::vector<std::shared_ptr<hittable_type>> objects_;
        };
    }
}

#endif // !_OHTOAI_HITTABLE_LIST_H_
