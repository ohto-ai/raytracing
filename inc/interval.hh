#pragma once
// interval.hh

#ifndef _OHTOAI_INTERVAL_H_
#define _OHTOAI_INTERVAL_H_

#include <numeric>

namespace ohtoai{
    namespace math {
        class Interval {
        public:
            constexpr Interval() = default;
            constexpr Interval(const Interval&) = default;
            constexpr Interval(Interval&&) = default;
            constexpr Interval& operator=(const Interval&) = default;
            constexpr Interval& operator=(Interval&&) = default;
            ~Interval() = default;

            constexpr Interval(real min, real max) : min_(min), max_(max) {}

            constexpr real min() const { return min_; }
            constexpr real max() const { return max_; }

            constexpr bool contains(real value) const {
                return !(value < min_ || max_ < value);
            }

            constexpr bool surrounds(real value) const {
                return min_ < value && value < max_;
            }

            constexpr bool is_empty() const {
                return min_ > max_;
            }

            constexpr real clamp(real value) const {
                return std::clamp(value, min_, max_);
            }

            protected:
                real min_;
                real max_;
        };

        constexpr Interval make_interval(real min, real max) {
            return Interval(min, max);
        }

        constexpr Interval make_interval(real value) {
            return Interval(value, value);
        }

        constexpr Interval EmptyInterval = make_interval(constants::infinity, -constants::infinity);

        constexpr Interval UniverseInterval = make_interval(-constants::infinity, constants::infinity);
    }
}

#endif // !_OHTOAI_INTERVAL_H_
