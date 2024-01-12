#pragma once
// type_base.hh

#ifndef _OHTOAI_TYPE_BASE_H_
#define _OHTOAI_TYPE_BASE_H_

#include <type_traits>
#include <limits>
#include <memory>

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
        }


        inline double degrees_to_radians(double degrees) {
            return degrees * constants::pi / 180.0;
        }
    }
}


#endif // !_OHTOAI_TYPE_BASE_H_
