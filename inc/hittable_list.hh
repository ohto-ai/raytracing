#pragma once
// hittable_list.hh

#ifndef _OHTOAI_HITTABLE_LIST_H_
#define _OHTOAI_HITTABLE_LIST_H_

#include "hittable.hh"

namespace ohtoai{
    namespace math {
        class HittableList : public Hittable {
        public:
            HittableList() = default;
            explicit HittableList(const std::shared_ptr<Hittable>& object) { add(object); }

            void clear() { objects_.clear(); }
            void add(const std::shared_ptr<Hittable>& object) { objects_.push_back(object); }

            virtual bool hit(const Ray& light, const Interval& ray_range, HitRecord& rec) const override {
                HitRecord temp_rec;
                bool hit_anything = false;
                auto closest_so_far = ray_range.max();

                for (const auto& object : objects_) {
                    if (object->hit(light, make_interval(ray_range.min(), closest_so_far), temp_rec)) {
                        hit_anything = true;
                        closest_so_far = temp_rec.t;
                        rec = temp_rec;
                    }
                }

                return hit_anything;
            }

        private:
            std::vector<std::shared_ptr<Hittable>> objects_;
        };
    }
}

#endif // !_OHTOAI_HITTABLE_LIST_H_
