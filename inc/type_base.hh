#pragma once
// type_base.hh

#ifndef _OHTOAI_TYPE_BASE_H_
#define _OHTOAI_TYPE_BASE_H_

#include <type_traits>
#include <limits>
#include <memory>
#include <random>
#if __cplusplus > 202002L
#include <numbers>
#endif

namespace ohtoai{
    namespace type {
        enum class endian
        {
        #if defined(_MSC_VER) && !defined(__clang__)
            little = 0,
            big    = 1,
            native = little
        #else
            little = __ORDER_LITTLE_ENDIAN__,
            big    = __ORDER_BIG_ENDIAN__,
            native = __BYTE_ORDER__
        #endif
        };
    }

    namespace math {
        namespace constants {
        // if cpp version > c++20, use std::numbers::pi
        #if __cplusplus > 202002L
            using std::numbers::pi;
        #else
            inline constexpr double pi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067;
        #endif
            inline constexpr auto infinity = std::numeric_limits<double>::infinity();
        }

        inline double degrees_to_radians(double degrees) {
            return degrees * constants::pi / 180.0;
        }

        static inline std::mt19937 random_gen = std::mt19937(std::random_device()());
        inline double random_double(std::mt19937& gen, double min, double max) {
            std::uniform_real_distribution<double> dis(min, max);
            return dis(gen);
        }
    }
}


#endif // !_OHTOAI_TYPE_BASE_H_
