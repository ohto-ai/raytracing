#pragma once
// interval.hh

#ifndef _OHTOAI_INTERVAL_H_
#define _OHTOAI_INTERVAL_H_

#include <numeric>

namespace ohtoai{
    namespace math {
        class Interval {
        public:
            Interval() = default;
            Interval(const Interval&) = default;
            Interval(Interval&&) = default;
            Interval& operator=(const Interval&) = default;
            Interval& operator=(Interval&&) = default;
            ~Interval() = default;

            Interval(real min, real max) : min_(min), max_(max) {}

            real min() const { return min_; }
            real max() const { return max_; }

            bool contains(real value) const {
                return !(value < min_ || max_ < value);
            }

            bool surrounds(real value) const {
                return min_ < value && value < max_;
            }

            bool is_empty() const {
                return min_ > max_;
            }

            real clamp(real value) const {
                return std::clamp(value, min_, max_);
            }

            protected:
                real min_;
                real max_;
        };

        Interval make_interval(real min, real max) {
            return Interval(min, max);
        }

        Interval make_interval(real value) {
            return Interval(value, value);
        }

        Interval EmptyInterval = make_interval(constants::infinity, -constants::infinity);

        Interval UniverseInterval = make_interval(-constants::infinity, constants::infinity);
    }
}

#endif // !_OHTOAI_INTERVAL_H_
