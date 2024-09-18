/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin, Vladyslav Merais
*/

#ifndef sixit_core_cpual_ieeefloat_inline_asm_h_included
#define sixit_core_cpual_ieeefloat_inline_asm_h_included

#include "sixit/core/core.h"

#include <cstring>
#include <type_traits>

namespace sixit::cpual
{

// @cond
#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)

#if defined(SIXIT_COMPILER_MSVC)
#include <xmmintrin.h>

using asm_float_t = ::__m128;

inline asm_float_t ieee_add_float(asm_float_t a, asm_float_t b)
{
    return _mm_add_ss(a, b);
}

inline asm_float_t ieee_subtract_float(asm_float_t a, asm_float_t b)
{
    return _mm_sub_ss(a, b);
}

inline asm_float_t ieee_multiply_float(asm_float_t a, asm_float_t b)
{
    // A hack related to '/fp:fast' or '/fp:contract' options below:
    // The compliler may fuse `x += a * b` into a single fmadd instruction, affecting determinism.
    // Workaround: make multiplication result made observable by broadcasting it into unused part of XMM register
    __m128 result = _mm_mul_ss(a, b);
    return _mm_movelh_ps(result, result);
}

inline asm_float_t ieee_divide_float(asm_float_t a, asm_float_t b)
{
    // A hack related to '/fp:fast' or '/fp:contract' options below:
    // The compliler may fuse `x += a * b` into a single fmadd instruction, affecting determinism.
    // Workaround: make multiplication result made observable by broadcasting it into unused part of XMM register
    __m128 result = _mm_div_ss(a, b);
    return _mm_movelh_ps(result, result);
}

/**
 * @brief `a` less-than `b`
 */
inline bool ieee_cmp_lt_float(asm_float_t a, asm_float_t b)
{
    return _mm_ucomilt_ss(a, b) != 0;
}

/**
 * @brief `a` is less or equal to `b`
 */
inline bool ieee_cmp_le_float(asm_float_t a, asm_float_t b)
{
    return _mm_ucomile_ss(a, b) != 0;
}

/**
 * @brief `a` is equal to `b`
 */
inline bool ieee_cmp_eq_float(asm_float_t a, asm_float_t b)
{
    return _mm_ucomieq_ss(a, b) != 0;
}

/**
 * @brief return -a;
 */
inline asm_float_t ieee_neg_float(asm_float_t a)
{
    static const __m128 sign_bit = _mm_set_ss(-0.f);
    return _mm_xor_ps(a, sign_bit);
}

inline float ieee_asm_to_float(asm_float_t intermediate)
{
    return _mm_cvtss_f32(intermediate);
}
constexpr inline asm_float_t ieee_asm_from_float(float f)
{
    if (std::is_constant_evaluated())
    {
        __m128 value = {};
        value.m128_f32[0] = f;
        return value;
    }
    else
    {
        return _mm_load_ss(&f);
    }
}
#elif defined(SIXIT_COMPILER_ANY_CLANG_OR_GCC)
using asm_float_t = float;
inline asm_float_t ieee_add_float(asm_float_t a, asm_float_t b)
{
    asm("addss %[rhs], %[a]" : [a] "+x"(a) : [rhs] "xm"(b));
    return a;
}
inline asm_float_t ieee_subtract_float(asm_float_t a, asm_float_t b)
{
    asm("subss %[rhs], %[a]" : [a] "+x"(a) : [rhs] "xm"(b));
    return a;
}
inline asm_float_t ieee_multiply_float(asm_float_t a, asm_float_t b)
{
    asm("mulss %[rhs], %[a]" : [a] "+x"(a) : [rhs] "xm"(b));
    return a;
}
inline asm_float_t ieee_divide_float(asm_float_t a, asm_float_t b)
{
    asm("divss %[rhs], %[a]" : [a] "+x"(a) : [rhs] "xm"(b));
    return a;
}

/**
 * @brief `a` less-than `b`
 */
inline bool ieee_cmp_lt_float(asm_float_t a, asm_float_t b)
{
    // tricky code: `ucomiss` sets flag not in the way int comparisons do, so the
    // conventional way is to implement it via `b > a`
    bool result;
    asm("ucomiss %[rhs], %[lhs] \n\t" // EFLAGS {ZF,PF,CF}: nan=111; less=001; greater=000; equal=100
        : "=@cca"(result)             // result = EFLAGS[ZF] == 0 && EFLAGS[CF] == 0;
        : [lhs] "x"(b), [rhs] "x"(a)  // ... i.e. `b > a`
        : "cc");
    return result;
}

/**
 * @brief `a` is less or equal to `b`
 */
inline bool ieee_cmp_le_float(asm_float_t a, asm_float_t b)
{
    // tricky code: `ucomiss` sets flag not in the way int comparisons do, so the
    // conventional way is to implement it via `b >= a`
    bool result;
    asm("ucomiss %[rhs], %[lhs] \n\t" // EFLAGS {ZF,PF,CF}: nan=111; less=001; greater=000; equal=100
        : "=@ccnb"(result)            // result = 'b' not below 'a' (CF=0)
        : [lhs] "x"(b), [rhs] "x"(a)  // ... i.e. `b >= a`
        : "cc");
    return result;
}

/**
 * @brief `a` is equal to `b`
 */
inline bool ieee_cmp_eq_float(asm_float_t a, asm_float_t b)
{
    int result = {};
    int zero = {};                       // let the compiler be in charge of register allocation to hold 0 constant
    asm("comiss %[rhs], %[lhs]     \n\t" // EFLAGS {ZF,PF,CF}: nan=111; less=001; greater=000; equal=100
        "setnp %b[result]          \n\t" // result=1, if neither is nan
        "cmovne %[zero], %[result] \n\t" // result=0, if not equal
        : [result] "+&r"(result)
        : [zero] "r"(zero), [lhs] "x"(a), [rhs] "x"(b)
        : "cc");
    return result;
}

/**
 * @brief return -a;
 */
inline asm_float_t ieee_neg_float(asm_float_t a)
{
    static const float sign_bit[4] = {-0.f}; // `static const` enables `xorps mem, reg`
    asm("xorps %[sign_bit], %[a] \n\t"       // xorps doesn't modify flags
        : [a] "+x"(a)
        : [sign_bit] "m"(sign_bit));
    return a;
}

inline float ieee_asm_to_float(asm_float_t intermediate)
{
    return intermediate;
}
constexpr inline asm_float_t ieee_asm_from_float(float f)
{
    return f;
}
#else
// not supported on this platform
#endif

#elif defined(SIXIT_CPU_ARM64)
using asm_float_t = float;
inline asm_float_t ieee_add_float(asm_float_t a, asm_float_t b)
{
    asm("fadd %s[lhs], %s[lhs], %s[rhs]" : [lhs] "+w"(a) : [rhs] "w"(b));
    return a;
}
inline asm_float_t ieee_subtract_float(asm_float_t a, asm_float_t b)
{
    asm("fsub %s[lhs], %s[lhs], %s[rhs]" : [lhs] "+w"(a) : [rhs] "w"(b));
    return a;
}
inline asm_float_t ieee_multiply_float(asm_float_t a, asm_float_t b)
{
    asm("fmul %s[lhs], %s[lhs], %s[rhs]" : [lhs] "+w"(a) : [rhs] "w"(b));
    return a;
}
inline asm_float_t ieee_divide_float(asm_float_t a, asm_float_t b)
{
    asm("fdiv %s[lhs], %s[lhs], %s[rhs]" : [lhs] "+w"(a) : [rhs] "w"(b));
    return a;
}

/**
 * @brief `a` less-than `b`
 */
inline bool ieee_cmp_lt_float(asm_float_t a, asm_float_t b)
{
    bool result;
    asm("fcmp %s[lhs], %s[rhs] \n\t"
        "cset %w[result], mi   \n\t"
        : [result] "=r"(result)
        : [lhs] "w"(a), [rhs] "w"(b)
        : "cc");
    return result;
}

/**
 * @brief `a` is less or equal to `b`
 */
inline bool ieee_cmp_le_float(asm_float_t a, asm_float_t b)
{
    bool result;
    asm("fcmp %s[lhs], %s[rhs] \n\t"
        "cset %w[result], ls   \n\t"
        : [result] "=r"(result)
        : [lhs] "w"(a), [rhs] "w"(b)
        : "cc");
    return result;
}

/**
 * @brief `a` is equal to `b`
 */
inline bool ieee_cmp_eq_float(asm_float_t a, asm_float_t b)
{
    bool result;
    asm("fcmp %s[lhs], %s[rhs] \n\t"
        "cset %w[result], eq   \n\t"
        : [result] "=r"(result)
        : [lhs] "w"(a), [rhs] "w"(b)
        : "cc");
    return result;
}

/**
 * @brief return -a;
 */
inline asm_float_t ieee_neg_float(asm_float_t a)
{
    asm("fneg %s[a], %s[a] \n\t" : [a] "+w"(a));
    return a;
}

inline float ieee_asm_to_float(asm_float_t intermediate)
{
    return intermediate;
}

constexpr inline asm_float_t ieee_asm_from_float(float f)
{
    return f;
}

#elif defined(SIXIT_CPU_RISCV64)

#if defined(__riscv_flen) && __riscv_flen >= 32

using asm_float_t = float;

inline asm_float_t ieee_add_float(asm_float_t a, asm_float_t b)
{
    asm("fadd.s %0, %0, %1" : "+f"(a) : "f"(b));
    return a;
}

inline asm_float_t ieee_subtract_float(asm_float_t a, asm_float_t b)
{
    asm("fsub.s %0, %0, %1" : "+f"(a) : "f"(b));
    return a;
}

inline asm_float_t ieee_multiply_float(asm_float_t a, asm_float_t b)
{
    asm("fmul.s %0, %0, %1" : "+f"(a) : "f"(b));
    return a;
}

inline asm_float_t ieee_divide_float(asm_float_t a, asm_float_t b)
{
    asm("fdiv.s %0, %0, %1" : "+f"(a) : "f"(b));
    return a;
}

/**
 * @brief `a` less-than `b`
 */
inline bool ieee_cmp_lt_float(asm_float_t a, asm_float_t b)
{
    bool result;
    asm("flt.s %0, %1, %2" : "=r"(result) : "f"(a), "f"(b));
    return result;
}

/**
 * @brief `a` is less or equal to `b`
 */
inline bool ieee_cmp_le_float(asm_float_t a, asm_float_t b)
{
    bool result;
    asm("fle.s %0, %1, %2" : "=r"(result) : "f"(a), "f"(b));
    return result;
}

/**
 * @brief `a` is equal to `b`
 */
inline bool ieee_cmp_eq_float(asm_float_t a, asm_float_t b)
{
    bool result;
    asm("feq.s %0, %1, %2" : "=r"(result) : "f"(a), "f"(b));
    return result;
}

/**
 * @brief return -a;
 */
inline asm_float_t ieee_neg_float(asm_float_t a)
{
    asm("fneg.s %0, %0" : "+f"(a));
    return a;
}

inline float ieee_asm_to_float(asm_float_t intermediate)
{
    return intermediate;
}
constexpr inline asm_float_t ieee_asm_from_float(float f)
{
    return f;
}

#else
#error Single precision floating point ABI is not supported
#endif

#else // inapplicable or an unknown CPU - default implementation + warning or so
// not supported on this platform
#pragma message SIXIT_WARNING("unknown architecture")
#endif
/// @endcond

} // namespace sixit::cpual

#endif // sixit_core_cpual_ieeefloat_inline_asm_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin, Vladyslav Merais

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