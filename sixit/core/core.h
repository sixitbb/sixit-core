/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Marcos Bracco
*/

/** \file core.h
\brief **sixit/core.h**: core file for everything sixit \emoji wink. Usually, you don't need to include it directly, but there is no harm in doing it. \emoji exclamation **IMPORTANT: we rely on C++20 quite heavily for the wholesixit:: project, so make sure to specify -std=c++20 or /std:c++20 in the command line of your compiler!** \emoji exclamation
*/

/* KEY POINTS REGARDING COMPILER, PLATFORM, AND OS                               */

#ifndef sixit_core_core_h_included
#define sixit_core_core_h_included

#include <climits> // for CHAR_BIT

static_assert(CHAR_BIT == 8);
static_assert(sizeof(char) == 1);

#ifdef SIXIT_DOXYGEN
//ONLY for documentation purposes! Will not compile under normal compiler!
/**
 * SIXIT_COMPILER_* - supported variants are:
- SIXIT_COMPILER_MSVC
- SIXIT_COMPILER_GCC (unlike `__GNUC__`, is NOT defined under Clang!)
- SIXIT_COMPILER_VANILLA_CLANG
- SIXIT_COMPILER_APPLE_CLANG
- SIXIT_COMPILER_ANY_CLANG
- SIXIT_COMPILER_ANY_CLANG_OR_GCC
- SIXIT_COMPILER_ANY_GCC_OR_CLANG (alias of SIXIT_COMPILER_ANY_CLANG_OR_GCC)
- PLANNED: SIXIT_COMPILER_ICX (but no legacy ICC)
- PLANNED: SIXIT_COMPILER_XLC (Clang-based ones)
- MAYBE: SIXIT_COMPILER_AOCC

All macros are integers, normalized to XXYY pattern, where XX is major version, and
YY is minor version, so for GCC 10.03 SIXIT_COMPILER_GCC will be defined to 1003

If none is detected - it is SIXIT_COMPILER_UNKNOWN
*/
#  define SIXIT_COMPILER_*

/**
 * SIXIT_CPU_* - supported variants are:
- SIXIT_CPU_X64
- SIXIT_CPU_X86 (for x86, as well as for any 32-bit CPUs w/o 64-bit ALU,
performance maybe suboptimal); **currently UNTESTED**
- SIXIT_CPU_ARM64
- SIXIT_CPU_WASM32 (NB: WASM32 does have 64-bit ALU, so sixit is reasonably-optimal
there)
- TODO: SIXIT_CPU_WASM64 (to be added when there is some VM with robust support)
- PLANNED: SIXIT_CPU_RISCV
- PLANNED: SIXIT_CPU_POWER
- PLANNED: SIXIT_CPU_S390X

There are no current plans to support discontinued CPUs such as SPARC, PA-RISC, Itanium,
Alpha, and others - but if you have patches which make us work there - feel free to
submit pull requests.
There are even less current plans to support MCUs (except maybe for ARM32-based ones),
but if you have patches which make us work there, AND those patches are reasonably-local
(=do NOT change codebase too much) - feel free to submit pull requests. Note though
that sixit is written with 64-bit arithmetic in mind, so its performance on MCUs may
suffer (see note above re. 64-bit ALU). Versions of the code optimized for 32-bit
(i.e. minimizing use of uint64_t's) may be submitted too - as long they do NOT
clutter the code with too many ifdefs.

If none of known CPUs is detected - it is SIXIT_CPU_UNKNOWN; it means that all
CPU-specific optimizations (those encapsulated in sixit/cpual, incl. SIMD) are disabled.
*/
#  define SIXIT_CPU_*

/**
 * SIXIT_OS_* - supported variants are:
- SIXIT_OS_WINDOWS
- SIXIT_OS_LINUX
- SIXIT_OS_MAC
- SIXIT_OS_ANDROID
- SIXIT_OS_EMSCRIPTEN_BROWSER
- SIXIT_OS_WASM_SANDBOX

If none is detected - it is SIXIT_OS_UNKNOWN
*/
#  define SIXIT_OS_*
#endif //SIXIT_DOXYGEN

//COMPILER
/// @cond
#ifdef _MSC_VER
#   define SIXIT_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#   ifdef __clang__
#       define SIXIT_COMPILER_ANY_CLANG
#       ifdef __apple_build_version__
#           define SIXIT_COMPILER_APPLE_CLANG ( (__apple_build_version__) / 10000 )
#       else
#           define SIXIT_COMPILER_VANILLA_CLANG ( (__clang_major__) * 100 + __clang_minor__)
#       endif// __apple_build_version__
#   else
//      !__clang__, probably "pure" GCC
#       define SIXIT_COMPILER_GCC ( (__GNUC__) * 100 + __GNUC_MINOR__)
#   endif

#   define SIXIT_COMPILER_ANY_CLANG_OR_GCC
#   define SIXIT_COMPILER_ANY_GCC_OR_CLANG
#else
#   define SIXIT_COMPILER_UNKNOWN
#endif
/// @endcond


//CPU
/// @cond
#if defined(__X86_64__) || defined(__X86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_X64)
#  define SIXIT_CPU_X64
static_assert(sizeof(void*) == 8);
#elif defined(__i386__) || defined(i386) || defined(__i386) || defined(__I86__) || defined(_M_IX86)
#  define SIXIT_CPU_X86
static_assert(sizeof(void*) == 4);
#elif defined(__arm64) || defined(arm64) || defined(__aarch64__)
#  define SIXIT_CPU_ARM64
static_assert(sizeof(void*) == 8);
#elif defined(__riscv)
#  if __riscv_xlen == 64
#    define SIXIT_CPU_RISCV64
static_assert(sizeof(void*) == 8);
#  elif __riscv_xlen == 32
#    define SIXIT_CPU_RISCV32
static_assert(sizeof(void*) == 4);
#  endif
#elif defined(__EMSCRIPTEN__)
#  if defined(__wasm32__) || defined(__wasm32)
#    define SIXIT_CPU_WASM32
static_assert(sizeof(void*) == 4);
#  elif defined(__wasm64__) || defined(__wasm64)
#    define SIXIT_CPU_WASM64
static_assert(sizeof(void*) == 8);
#  else
#    define SIXIT_CPU_UNKNOWN
#  endif
#else
#  define SIXIT_CPU_UNKNOWN
#endif
/// @endcond

#if defined(__OS_WASM_SANDBOX__)
#  define SIXIT_OS_WASM_SANDBOX
#endif

//OS
/// @cond
#if defined(__ANDROID__)
//mb: while android is a kind of linux, it has its own libc (Bionic) so we need to diferentiate
#  define SIXIT_OS_ANDROID
#elif (defined __linux) || (defined linux) || (defined __linux__)
#  define SIXIT_OS_LINUX
#elif (defined __WINDOWS__) || (defined _WIN32) || (defined _WIN64)
#  define SIXIT_OS_WINDOWS
#elif (defined __OSX__) || (defined __APPLE__)
#  include <TargetConditionals.h>
#  if TARGET_IPHONE_SIMULATOR
#    define SIXIT_OS_IOS
#    define SIXIT_OS_IOS_SIMULATOR
#  elif TARGET_OS_IPHONE || TARGET_OS_IOS // iOS, tvOS, or watchOS device
#    define SIXIT_OS_IOS
#    define SIXIT_OS_IOS_IOS
#  elif TARGET_OS_MACCATALYST // Mac's Catalyst (ports iOS API into Mac, like UIKit).
#    define SIXIT_OS_IOS
#    define SIXIT_OS_IOS_MACCATALYST
#  elif TARGET_OS_MAC
#    define SIXIT_OS_MAC
#else
#   error "Unknown Apple platform"
#endif
#elif (defined __EMSCRIPTEN__)
#  define SIXIT_OS_EMSCRIPTEN_BROWSER
#else
#  define SIXIT_OS_UNKNOWN
#endif
/// @endcond

// !!!NO COMPILER-SPECIFIC #defines PAST THIS POINT!!!

/// @cond
//we will have to warn here and there, so let's define a way of doing it as early as possible
#define SIXIT_INTERNAL_STRINGIFY_IMPL(x) #x
#define SIXIT_INTERNAL_STRINGIFY(x) SIXIT_INTERNAL_STRINGIFY_IMPL(x)
#define SIXIT_INTERNAL_CONCATENATE_TOKENS_IMPL(T1, T2) T1 ## T2
#define SIXIT_INTERNAL_CONCATENATE_TOKENS(T1, T2) SIXIT_INTERNAL_CONCATENATE_TOKENS_IMPL(T1, T2)
/// @endcond

#ifdef SIXIT_DOXYGEN
/** \brief Usage: `#pragma message SIXIT_WARNING("This is your last warning!")`

 NB: it is a temporary workaround until C++23 `#warning` is finally supported across the board.
 */
#define SIXIT_WARNING(msg)
#endif
/// @cond
//SIXIT_WARNING - inspired by https://stackoverflow.com/a/1911632/4947867
//#pragma message SIXIT_WARNING("") are to be replaced with #warning
//                THROUGHOUT ALL THE CODE as soon as MSVC implements P2437R1
#ifdef SIXIT_COMPILER_ANY_CLANG_OR_GCC
#  define SIXIT_WARNING(msg) ("SIXIT_WARNING: " msg)
#elif defined(SIXIT_COMPILER_MSVC)
#  define SIXIT_WARNING(msg) (__FILE__ "(" SIXIT_INTERNAL_STRINGIFY(__LINE__) ") : SIXIT_WARNING: " msg)
#else //unknown compiler
#  define SIXIT_WARNING(msg) ("SIXIT_WARNING: " msg)
#endif
/// @endcond

//warnings about untested compilers
#ifdef SIXIT_COMPILER_VANILLA_CLANG
#  if SIXIT_COMPILER_VANILLA_CLANG < 1200
#    define SIXIT__COUNTER__OR__LINE __LINE__
#    pragma message SIXIT_WARNING("we do not test our code on mainstream Clang under 12.0")
#  endif
#elif defined(SIXIT_COMPILER_APPLE_CLANG)
#  if SIXIT_COMPILER_VANILLA_CLANG < 1300
#    define SIXIT__COUNTER__OR__LINE __LINE__
#    pragma message SIXIT_WARNING("we do not test our code on Apple Clang under 13.0")
#  endif
#elif defined(SIXIT_COMPILER_GCC)
#  if SIXIT_COMPILER_GCC < 1003
#    define SIXIT__COUNTER__OR__LINE __LINE__
#    pragma message SIXIT_WARNING("we do not test our code on GCC under 10.3")
#  endif
#elif defined(SIXIT_COMPILER_MSVC)
#  if SIXIT_COMPILER_MSVC < 1928
#    define SIXIT__COUNTER__OR__LINE __LINE__
#    pragma message SIXIT_WARNING("we do not test our code on MSVC under 19.28")
#  endif
#else
#  define SIXIT_COMPILER_UNKNOWN
#pragma message SIXIT_WARNING("unknown compiler. HIC SVNT LEONES")
#endif

//warnings about unsupported platforms
#ifdef SIXIT_CPU_UNKNOWN
#  error unknown/unsupported CPU
#elif defined(SIXIT_CPU_WASM64)
#  pragma message SIXIT_WARNING("we do not test our code for wasm64 which is barely supported at present")
#endif

// some common staff

#ifdef SIXIT_DOXYGEN
/// Forcing inlining (if compiler supports it). Use in lieu of `inline`
#define SIXIT_FORCEINLINE
/// Forcing NOT inlining (if compiler supports it). Use instead of `inline`
#define SIXIT_NOINLINE
#endif
/// @cond
#if defined(SIXIT_COMPILER_MSVC)
#  define SIXIT_NOINLINE      __declspec(noinline)
#  define SIXIT_FORCEINLINE    __forceinline
#elif defined (SIXIT_COMPILER_ANY_CLANG_OR_GCC)
#  define SIXIT_NOINLINE      __attribute__ ((noinline))
#  define SIXIT_FORCEINLINE inline __attribute__((always_inline))
#else
#  define SIXIT_FORCEINLINE inline
#  define SIXIT_NOINLINE
#endif
/// @endcond

// compile-time warning about user-defined condition and message
namespace sixit::core { // TODO namespace
    class detail {
      public:
        template<bool condition>
        struct ws_impl {
            template<class L>
            constexpr ws_impl(L&& l) {
                if constexpr (condition)
                    typename decltype(l())::w w;
            }
        };
    };
} // sixit::core

// calling this will result, if the con is true in a compile-time warning with 'msg' text in it
#define SIXIT_STATIC_WARNING_3( name, cond, msg )	\
    static constexpr ::sixit::core::detail::ws_impl<(cond)> name = ::sixit::core::detail::ws_impl<(cond)>( []() constexpr { struct WARNING { [[deprecated(msg)]] constexpr WARNING() {} }; struct W{ using w = WARNING; }; return W(); } );

/// @cond
#ifndef SIXIT__COUNTER__OR__LINE
#  ifdef SIXIT_COMPILER_UNKNOWN
#    define SIXIT__COUNTER__OR__LINE __LINE__
#  else
#    define SIXIT__COUNTER__OR__LINE __COUNTER__
#  endif
#endif
/// @endcond


#ifdef SIXIT_DOXYGEN
/** Macro to generate warning if condition on template parameter is false.
Admittedly hacky:
- is allowed only in class scope or in function scope
- creates a global object - which prints warning to console in its constructor (OMG)

Still better than nothing...

Usage example:
```
template <class T>
class Foo {
  SIXIT_STATIC_WARNING(std::is_integral_v<T>,"should be integral type here");
};
```
*/
#define SIXIT_STATIC_WARNING(cond, msg)
#endif //SIXIT_DOXYGEN

// simpler form of SIXIT_STATIC_WARNING_3
/// @cond
#define SIXIT_STATIC_WARNING( cond, msg )	\
    SIXIT_STATIC_WARNING_3( SIXIT_INTERNAL_CONCATENATE_TOKENS(name_, SIXIT__COUNTER__OR__LINE), (cond), msg )
/// @endcond

// @cond
#ifdef SIXIT_MT
#  if SIXIT_MT!=0 && SIXIT_MT!=1
#    error "SIXIT_MT MUST be 0 or 1"
#  endif
#else
#  ifdef SIXIT_COMPILER_ANY_CLANG_OR_GCC /* GCC or Clang */
#    if defined(_REENTRANT) || defined(SIXIT_OS_MAC)
#      define SIXIT_MT 1
#    else
#      define SIXIT_MT 0
#    endif
#  endif
#  ifdef SIXIT_COMPILER_MSVC
#    ifdef _MT
#      define SIXIT_MT 1
#    else
#      define SIXIT_MT 0
#    endif
#  endif
#  ifndef SIXIT_MT /* unknown compiler */
#    define SIXIT_MT 1 /* being MT is MUCH safer */
#  endif
#endif
// @endcond

#endif //sixit_core_core_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Marcos Bracco

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
