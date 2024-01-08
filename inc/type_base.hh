#pragma once
// type_base.hh

#ifndef _OHTOAI_TYPE_BASE_H_
#define _OHTOAI_TYPE_BASE_H_

#include <type_traits>

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
}


#endif // !_OHTOAI_TYPE_BASE_H_
