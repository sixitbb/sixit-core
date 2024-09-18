/*
Copyright (C) 2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Vladyslav Merais, Victor Istomin, Dmytro Ivanchykhin
*/

#ifndef sixit_core_cpual_clock_count_h_included
#define sixit_core_cpual_clock_count_h_included

#include "sixit/core/core.h"
#include "sixit/core/guidelines.h"

#include <array>
#include <cstdint>

#if defined(SIXIT_COMPILER_MSVC)
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#elif defined(SIXIT_COMPILER_ANY_GCC_OR_CLANG) && (defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86))
#include <cpuid.h>
#include <x86intrin.h>
#endif

namespace sixit::cpual
{


#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)

#  if defined(SIXIT_COMPILER_MSVC)
    constexpr bool is_clock_count_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t clock_count()
    {
        return __rdtsc();
    }
#  elif defined(SIXIT_COMPILER_ANY_GCC_OR_CLANG)
    constexpr bool is_clock_count_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t clock_count()
    {
        return __builtin_ia32_rdtsc();
    }
#  else
    constexpr bool is_clock_count_supported = false;
    static SIXIT_FORCEINLINE std::uint64_t clock_count()
    {
        return 0;
    }
#  endif

#elif defined(SIXIT_CPU_RISCV64)
    constexpr bool is_clock_count_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t clock_count()
    {
        std::uint64_t time;
        asm volatile("rdcycle %0" : "=r"(time));
        return time;
    }
#elif defined(SIXIT_CPU_ARM64)
    constexpr bool is_clock_count_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t clock_count()
    {
        std::uint64_t time;
        asm volatile("isb" : : : "memory");
        asm volatile("mrs %0, cntvct_el0" : "=r"(time));
        return time;
    }
#else
    constexpr bool is_clock_count_supported = false;
    static SIXIT_FORCEINLINE std::uint64_t clock_count()
    {
        return 0;
    }
#endif


#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)

#  if defined(SIXIT_COMPILER_MSVC)
    constexpr bool is_cpu_timer_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t cpu_timer()
    {
        return __rdtsc();
    }
#  elif defined(SIXIT_COMPILER_ANY_GCC_OR_CLANG)
    constexpr bool is_cpu_timer_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t cpu_timer()
    {
        return __builtin_ia32_rdtsc();
    }
#  else
    constexpr bool is_cpu_timer_supported = false;
    static SIXIT_FORCEINLINE std::uint64_t cpu_timer()
    {
        return 0;
    }
#  endif

#elif defined(SIXIT_CPU_RISCV64)
    constexpr bool is_cpu_timer_supported = true;
    static SIXIT_FORCEINLINE std::uint64_t cpu_timer()
    {
        std::uint64_t time;
        asm volatile("rdtime %0" : "=r"(time));
        return time;
    }
#elif defined(SIXIT_CPU_ARM64)
static SIXIT_FORCEINLINE std::uint64_t cpu_timer()
{
    constexpr bool is_cpu_timer_supported = true;
    std::uint64_t time;
    asm volatile("isb" : : : "memory");
    asm volatile("mrs %0, cntvct_el0" : "=r"(time));
    return time;
}
#else
    constexpr bool is_cpu_timer_supported = false;
    static SIXIT_FORCEINLINE std::uint64_t cpu_timer()
    {
        return 0;
    }
#endif


#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)

static SIXIT_FORCEINLINE std::array<int, 4> cpuid(uint8_t eax_value)
{
    std::array<int, 4> registers{};

#if defined(SIXIT_COMPILER_MSVC)
    __cpuid(registers.data(), eax_value);
#elif defined(SIXIT_COMPILER_ANY_GCC_OR_CLANG)
    asm volatile("cpuid"
                 : "=a"(registers[0]), "=b"(registers[1]), "=c"(registers[2]), "=d"(registers[3])
                 : "a"(eax_value), "c"(0));
#else
#error "Unsupported compiler for get cpuid in x86/x64"
#endif

    return registers;
}

static uint64_t get_cpu_art_value_by_model(const uint32_t model)
{
    constexpr std::pair<uint32_t, uint64_t> intel_fam6_kabylake = {0x9E, 24000000};
    constexpr std::pair<uint32_t, uint64_t> intel_fam6_kabylake_l = {0x8E, 24000000};

    constexpr std::pair<uint32_t, uint64_t> intel_fam6_skylake = {0x5E, 24000000};
    constexpr std::pair<uint32_t, uint64_t> intel_fam6_skylake_l = {0x4E, 24000000};

    constexpr std::pair<uint32_t, uint64_t> intel_fam6_cometlake = {0xA5, 24000000};
    constexpr std::pair<uint32_t, uint64_t> intel_fam6_cometlake_l = {0xA6, 24000000};

    constexpr std::pair<uint32_t, uint64_t> intel_fam6_atom_goldmont = {0x5C, 19200000};
    constexpr std::pair<uint32_t, uint64_t> intel_fam6_atom_goldmont_plus = {0x7A, 19200000};
    constexpr std::pair<uint32_t, uint64_t> intel_fam6_atom_goldmont_d = {0x5F, 25000000};

    switch (model)
    {
    case intel_fam6_kabylake.first:
        return intel_fam6_kabylake.second;
    case intel_fam6_kabylake_l.first:
        return intel_fam6_kabylake_l.second;
    case intel_fam6_skylake.first:
        return intel_fam6_skylake.second;
    case intel_fam6_skylake_l.first:
        return intel_fam6_skylake_l.second;
    case intel_fam6_cometlake.first:
        return intel_fam6_cometlake.second;
    case intel_fam6_cometlake_l.first:
        return intel_fam6_cometlake_l.second;
    case intel_fam6_atom_goldmont.first:
        return intel_fam6_atom_goldmont.second;
    case intel_fam6_atom_goldmont_plus.first:
        return intel_fam6_atom_goldmont_plus.second;
    case intel_fam6_atom_goldmont_d.first:
        return intel_fam6_atom_goldmont_d.second;
    default:
        return 0;
    }
}

static uint64_t detect_cpu_art_value()
{
    const auto registers = cpuid(0x1);
    const int eax_value = registers[0];

    uint32_t model = (eax_value >> 4) & 0xf;
    int32_t family = (eax_value >> 8) & 0xf;

    // check for extended family
    if (family == 0xf)
    {
        family += (eax_value >> 20) & 0xff;
    }

    // check for extended model
    if (family >= 6)
    {
        model += ((eax_value >> 16) & 0xf) << 4; // gets full model id
    }

    return get_cpu_art_value_by_model(model);
}

#endif // #if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)

static std::uint64_t clock_count_frequency()
{
#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86)

    const auto registers = cpuid(0x15);

    const uint64_t eax_value = registers[0];
    const uint64_t ebx_value = registers[1];
    const uint64_t ecx_value = registers[2];

    if (eax_value == 0 || ebx_value == 0)
    {
        return 0;
    }

    const uint64_t art_value = ecx_value == 0 ? detect_cpu_art_value() : ecx_value;

    if (art_value == 0)
    {
        return 0;
    }

    return (art_value * ebx_value) / eax_value;

#elif defined(SIXIT_CPU_RISCV32) || defined(SIXIT_CPU_RISCV64)

    return 0;

#elif defined(SIXIT_CPU_ARM64)

    uint64_t frequency;
    asm volatile("isb" : : : "memory");
    asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    return frequency;

#else

    return 0;

#endif
}

} // namespace sixit::cpual

#endif // sixit_core_cpual_clock_count_h_included

/*
The 3-Clause BSD License

Copyright (C) 2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Vladyslav Merais, Victor Istomin, Dmytro Ivanchykhin

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