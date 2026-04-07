#pragma once
// interval.hh

#ifndef _OHTOAI_INTERVAL_H_
#define _OHTOAI_INTERVAL_H_

#include <numeric>

namespace ohtoai{
    namespace math {
        class Interval {
        public:
            OHTOAI_HOST_DEVICE Interval() = default;
            OHTOAI_HOST_DEVICE Interval(const Interval&) = default;
            OHTOAI_HOST_DEVICE Interval(Interval&&) = default;
            OHTOAI_HOST_DEVICE Interval& operator=(const Interval&) = default;
            OHTOAI_HOST_DEVICE Interval& operator=(Interval&&) = default;
            OHTOAI_HOST_DEVICE ~Interval() = default;

            OHTOAI_HOST_DEVICE Interval(real min, real max) : min_(min), max_(max) {}

            OHTOAI_HOST_DEVICE real min() const { return min_; }
            OHTOAI_HOST_DEVICE real max() const { return max_; }

            OHTOAI_HOST_DEVICE bool contains(real value) const {
                return !(value < min_ || max_ < value);
            }

            OHTOAI_HOST_DEVICE bool surrounds(real value) const {
                return min_ < value && value < max_;
            }

            OHTOAI_HOST_DEVICE bool is_empty() const {
                return min_ > max_;
            }

            OHTOAI_HOST_DEVICE real clamp(real value) const {
                return std::clamp(value, min_, max_);
            }

            protected:
                real min_;
                real max_;
        };

        OHTOAI_HOST_DEVICE inline Interval make_interval(real min, real max) {
            return Interval(min, max);
        }

        OHTOAI_HOST_DEVICE inline Interval make_interval(real value) {
            return Interval(value, value);
        }

        inline Interval EmptyInterval    = make_interval(constants::infinity, -constants::infinity);
        inline Interval UniverseInterval = make_interval(-constants::infinity, constants::infinity);
    }
}

#endif // !_OHTOAI_INTERVAL_H_
