#pragma once

#ifndef ARX_TYPE_TRAITS_HAS_LIBSTDCPLUSPLUS_H
#define ARX_TYPE_TRAITS_HAS_LIBSTDCPLUSPLUS_H

#if !defined(ARX_HAVE_LIBSTDCPLUSPLUS)
    #if ARX_SYSTEM_HAS_INCLUDE(<cstdlib>) && !defined(ARDUINO_spresense_ast)
        #include <cstdlib>
        #if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
            // For gcc's libstdc++ and clang's libc++, assume that
            // __cplusplus tells us what the standard includes support
            #define ARX_HAVE_LIBSTDCPLUSPLUS __cplusplus
        #elif defined(_CPPLIB_VER)
            #if _CPPLIB_VER > 650
                #define ARX_HAVE_LIBSTDCPLUSPLUS 201703L  // C++17
            #elif _CPPLIB_VER == 650
                #define ARX_HAVE_LIBSTDCPLUSPLUS 201402L  // C++14
            #elif _CPPLIB_VER >= 610
                #define ARX_HAVE_LIBSTDCPLUSPLUS 201103L  // C++11
            #else
                #define ARX_HAVE_LIBSTDCPLUSPLUS 199711L  // C++98
            #endif
        #elif defined(__UCLIBCXX_MAJOR__)
            // For uclibc++, assume C++98 support only.
            #define ARX_HAVE_LIBSTDCPLUSPLUS 199711L
        #else
            #error "Unknown C++ library found, please report a bug against the ArxTypeTraits library about this."
        #endif
    #else
        // Assume no standard library is available at all (e.g. on AVR)
        #define ARX_HAVE_LIBSTDCPLUSPLUS 0
    #endif
#endif

#endif // ARX_TYPE_TRAITS_HAS_LIBSTDCPLUSPLUS_H
