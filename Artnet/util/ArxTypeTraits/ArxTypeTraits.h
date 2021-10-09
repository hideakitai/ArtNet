#pragma once

#ifndef ARX_TYPE_TRAITS_H
#define ARX_TYPE_TRAITS_H

#include "ArxTypeTraits/has_include.h"
#include "ArxTypeTraits/has_libstdcplusplus.h"

// Make sure std namespace exists
namespace std { }

// Import everything from the std namespace into arx::std, so that
// anything we import rather than define is also available through
// arx::stdx.
// This includes everything yet to be defined, so we can do this early
// (and must do so, to allow e.g. the C++14 additions in the arx::std
// namespace to reference the C++11 stuff from the system headers.
namespace arx {
    namespace stdx {
        using namespace ::std;
    }
}

// Import everything from arx::std back into the normal std namespace.
// This ensures that you can just use `std::foo` everywhere and you get
// the standard library version if it is available, falling back to arx
// versions for things not supplied by the standard library. Only when
// you really need the arx version (e.g. for constexpr numeric_limits
// when also using ArduinoSTL), you need to qualify with arx::stdx::
namespace std {
    using namespace ::arx::stdx;
}

#include "ArxTypeTraits/replace_minmax_macros.h"

#include "ArxTypeTraits/type_traits.h"

#endif // ARX_TYPE_TRAITS_H
