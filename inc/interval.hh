#pragma once
// interval.hh

#ifndef _OHTOAI_INTERVAL_H_
#define _OHTOAI_INTERVAL_H_

#include <numeric>

namespace ohtoai{
    namespace math {
        template<typename T>
        class Interval {
        public:
            using value_type = T;

            constexpr Interval() = default;
            constexpr Interval(const Interval&) = default;
            constexpr Interval(Interval&&) = default;
            constexpr Interval& operator=(const Interval&) = default;
            constexpr Interval& operator=(Interval&&) = default;
            ~Interval() = default;

            constexpr Interval(value_type min, value_type max) : min_(min), max_(max) {}

            constexpr value_type min() const { return min_; }
            constexpr value_type max() const { return max_; }

            constexpr bool contains(value_type value) const {
                return !(value < min_ || max_ < value);
            }

            constexpr bool surrounds(value_type value) const {
                return min_ < value && value < max_;
            }

            constexpr bool is_empty() const {
                return min_ > max_;
            }

            constexpr value_type clamp(value_type value) const {
                return std::clamp(value, min_, max_);
            }

            protected:
                value_type min_;
                value_type max_;
        };

        template<typename T>
        constexpr Interval<T> make_interval(T min, T max) {
            return Interval<T>(min, max);
        }

        template<typename T>
        constexpr Interval<T> make_interval(T value) {
            return Interval<T>(value, value);
        }

        template<typename T>
        constexpr Interval<T> EmptyInterval {
            std::numeric_limits<T>::infinity, -std::numeric_limits<T>::infinity
        };

        template<typename T>
        constexpr Interval<T> UniverseInterval {
            -std::numeric_limits<T>::infinity, std::numeric_limits<T>::infinity
        };
    }
}

#endif // !_OHTOAI_INTERVAL_H_
