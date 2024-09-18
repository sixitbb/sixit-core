/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin, Vladyslav Merais
*/

#ifndef sixit_core_cpual_cpual_h_included
#define sixit_core_cpual_cpual_h_included

#include "sixit/core/core.h"

#include <type_traits>
#include <cstring>
#include <cstddef>

namespace sixit::cpual
{

// @cond
#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)
    constexpr std::size_t sixit_cache_line_size = 64;
#elif defined(SIXIT_CPU_ARM64)
    constexpr std::size_t sixit_cache_line_size = 64;
#else // inapplicable or an unknown platform - just guessing
    constexpr std::size_t sixit_cache_line_size = 64;
#endif
/// @endcond


template <class T>
class alignas(cpual::sixit_cache_line_size) avoid_false_sharing
{
    using ValueT = T;
    static_assert(!std::is_same_v<T, void>);
    static constexpr size_t cacheline_cnt = (sizeof(T) - 1) / cpual::sixit_cache_line_size + 1;
    static constexpr size_t total_sz = cacheline_cnt * cpual::sixit_cache_line_size;
    static_assert(sizeof(T) <= total_sz);
    T t_;
    unsigned char padding[total_sz - sizeof(T)];

  public:
    T& operator()()
    {
        return t_;
    }
    const T& operator()() const
    {
        return t_;
    }
};

template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_pointer_v<T>, bool> = true>
T unaligned_load(std::byte* p) // in doxygen: emph potentially unaligned
{
#if defined SIXIT_CPU_X64
    return *reinterpret_cast<T*>(p);
#elif defined SIXIT_CPU_ARM64
    return *reinterpret_cast<T*>(p); // relying on https://stackoverflow.com/questions/38535738/does-aarch64-support-unaligned-access/38551472#38551472
#elif defined SIXIT_CPU_WASM64
#  ifdef SAFE_HEAP
#    if SAFE_HEAP == 1
    SIXIT_STATIC_WARNING(true, "generic support only is provided for this platform (performance issue) until testing is available");
    T buff;
    memcpy(&buff, p, sizeof(T));
    return buff;
#    else
    SIXIT_STATIC_WARNING(true, "generic support only is provided for this platform (performance issue) until testing is available");
    T buff;
    memcpy(&buff, p, sizeof(T));
    return buff;
#    endif
#  else
    SIXIT_STATIC_WARNING(true, "generic support only is provided for unknown platform (performance issue)");
    T buff;
    memcpy(&buff, p, sizeof(T));
    return buff;
#  endif
#elif defined SIXIT_CPU_WASM32
    SIXIT_STATIC_WARNING(true, "generic support only is provided for unknown platform (performance issue)");
    T buff;
    memcpy(&buff, p, sizeof(T));
    return buff;
#else
    SIXIT_STATIC_WARNING(true, "generic support only is provided for unknown platform (performance issue)");
    T buff;
    memcpy(&buff, p, sizeof(T));
    return buff;
#endif
}

template<typename T, std::enable_if_t<std::is_arithmetic_v<T> || std::is_pointer_v<T>, bool> = true>
void unaligned_store(std::byte * p, T t) // in doxygen: emph potentially unaligned
{
#if defined SIXIT_CPU_X64
    *reinterpret_cast<T*>(p) = t;
#elif defined SIXIT_CPU_ARM64
    * reinterpret_cast<T*>(p) = t; // relying on https://stackoverflow.com/questions/38535738/does-aarch64-support-unaligned-access/38551472#38551472
#elif defined SIXIT_CPU_WASM64
#  ifdef SAFE_HEAP
#    if SAFE_HEAP == 1
    SIXIT_STATIC_WARNING(true, "generic support only is provided for this platform (performance issue) until testing is available");
    memcpy(p, &t, sizeof(T));
#    else
    SIXIT_STATIC_WARNING(true, "generic support only is provided for this platform (performance issue) until testing is available");
    memcpy(p, &t, sizeof(T));
#    endif
#  else
    SIXIT_STATIC_WARNING(true, "generic support only is provided for unknown platform (performance issue)");
    memcpy(p, &t, sizeof(T));
#  endif
#elif defined SIXIT_CPU_WASM32
    SIXIT_STATIC_WARNING(true, "generic support only is provided for unknown platform (performance issue)");
    memcpy(p, &t, sizeof(T));
#else
    SIXIT_STATIC_WARNING(true, "generic support only is provided for unknown platform (performance issue)");
    memcpy(p, &t, sizeof(T));
#endif
}


} // namespace sixit::cpual

#endif //sixit_core_cpual_cpual_h_included

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