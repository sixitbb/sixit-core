/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_core_cpual_integer_math_h_included
#define sixit_core_cpual_integer_math_h_included

#include <stdint.h>
#include <bit>

#include "sixit/core/core.h"
#include "sixit/core/guidelines.h"

#ifdef SIXIT_COMPILER_MSVC
#include <intrin.h> // for _mul128
#endif

namespace sixit::core::cpual 
{
    struct uint128_t
    {
        uint128_t() : low(0), high(0) {}
        uint128_t(const uint64_t& l, const uint64_t& h) : low(l), high(h) {}; 

        uint64_t low;
        uint64_t high;  
    };

    struct int128_t
    {
        int128_t(): low(0), high(0) {}
        int128_t(const int64_t& l, const int64_t h): low(l), high(h) {}

        uint64_t low;
        int64_t high;
    };

    SIXIT_FORCEINLINE
    void umult64x64_default(const uint64_t& a, const uint64_t& b, uint64_t& low, uint64_t& high)
    {
        uint32_t* a_arr = (uint32_t*)(&a);
        uint32_t* b_arr = (uint32_t*)(&b);

        low = uint64_t(a_arr[0]) * b_arr[0];
        high = uint64_t(a_arr[1]) * b_arr[1];

        uint64_t middle(0);
        uint64_t tmp(0);

        middle = uint64_t(a_arr[0]) * b_arr[1];

        tmp = middle;
        middle += uint64_t(a_arr[1]) * b_arr[0];

        high += uint64_t(middle < tmp) << 32;

        tmp = low;
        low += middle << 32;

        high += (middle >> 32) + (low < tmp);
    }

    SIXIT_FORCEINLINE
    void mult64x64_defaul(const int64_t& a, const int64_t& b, uint64_t& low, int64_t& high)
    {
        uint64_t h;
        umult64x64_default(sixit::guidelines::bit_cast<uint64_t>(a), sixit::guidelines::bit_cast<uint64_t>(b), low, h);
        h -= (a < 0LL) * b + (b < 0LL) * a;
        high = sixit::guidelines::bit_cast<int64_t>(h);
    }


    SIXIT_FORCEINLINE
    uint128_t umul64x64(const uint64_t& a, const uint64_t& b)
    {
        uint128_t rv;
#ifdef SIXIT_COMPILER_ANY_CLANG_OR_GCC
        __uint128_t tmp_rv = __uint128_t(a) * b;
        rv.high = tmp_rv >> 64;
        rv.low = tmp_rv;
#elif defined(SIXIT_COMPILER_MSVC)
#ifdef SIXIT_CPU_ARM64
        rv.high = __umulh(a, b);
        rv.low = a * b;
#elif defined(SIXIT_CPU_X64)
        rv.low = _umul128(a, b, &rv.high);
#endif
#else
        umult64x64_default(a, b, rv.low, rv.high);
#endif
        return rv;        
    }

    SIXIT_FORCEINLINE
    int128_t mul64x64(const int64_t& a, const int64_t& b)
    {
        int128_t rv;
#ifdef SIXIT_COMPILER_ANY_CLANG_OR_GCC
        __int128_t tmp_rv = __int128(a) * b;
        rv.high = tmp_rv >> 64;
        rv.low = tmp_rv;
#elif defined(SIXIT_COMPILER_MSVC)
#ifdef SIXIT_CPU_ARM64
        rv.high = __mulh(a, b);
        rv.low = sixit::guidelines::bit_cast<uint64_t>(a * b);
#elif defined(SIXIT_CPU_X64)
        rv.low = sixit::guidelines::bit_cast<uint64_t>(_mul128(a, b, &rv.high));
#endif
#else
        mult64x64_default(a, b, rv.low, rv.high);
#endif
        return rv;
    }
}

#endif //sixit_core_cpual_integer_math_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Serhii Iliukhin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/