/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Guy Davidson, Dmytro Ivanchykhin, Victor Istomin, Marcos Bracco, Vladyslav Merais
*/

#ifndef sixit_core_guidelines_h_included
#define sixit_core_guidelines_h_included

#include <assert.h>
#include <cstddef>
#include <limits>
#include <memory>
#include <cmath>
#include <type_traits>
#include <typeindex>     // for HiddenParam
#include <unordered_map> // for HiddenParam

#include "sixit/core/lwa.h"

/** \file sixit::guidelines.h
 \brief **sixit/guidelines.h**: Contains primitives which support our own coding sixit::guidelines; while we do NOT force them down your throat, feel free to use them \emoji wink. */

/**
 @name ASSUMERT
 ASSUMERT conveys an idea that in Debug, we check the condition - but at Release, we allow compiler to rely on
 _exactly_ the same condition, which allows it to generate **potentially faster code** for Release.
 
 in general, we should replace assert()'s with ASSUMERT()'s throughout the whole codebase.
 */
#ifdef SIXIT_DOXYGEN
/// `assert(expr)` in Debug, `[[assume(expr)]]` (or equivalent) in Release
#  define SIXIT_ASSUMERT(expr)
/// `assert(false)` in Debug, `unreachable()` (or equivalent) in Release
#  define SIXIT_ASSUMERT_UNREACHABLE
#endif
/// @cond
#ifdef NDEBUG
#  define SIXIT_ASSUMERT(expr) SIXIT_LWA_ASSUME(expr)
#  define SIXIT_ASSUMERT_UNREACHABLE SIXIT_LWA_ASSUME_UNREACHABLE
#else
#  define SIXIT_ASSUMERT(expr) assert(expr)
#  define SIXIT_ASSUMERT_UNREACHABLE assert(0)
#endif
/// @endcond
/// @}

namespace sixit::guidelines
{
/** @name Type Helpers
   Minor extensions to std::is\_\* type helpers
  @{
*/

/** \hideinitializer
 \brief helper to avoid writing multiple std::is\_same\_v...
 */
template <typename T, typename... U>
inline constexpr bool is_any_of_v = (std::is_same_v<T, U> || ...);

template <typename T, typename U>
inline constexpr bool is_same_ignoring_cv_v = std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>;

/** \hideinitializer
 \brief helper to avoid writing multiple std::is\_same\_ignoring\_cv\_v...
 */
template <typename T, typename... U>
inline constexpr bool is_any_of_ignoring_cv_v = (is_same_ignoring_cv_v<T, U> || ...);
/** @} */

/// @cond
template <typename OutputT>
constexpr bool is_representable(auto value)
{
    using InputT = decltype(value);
    if constexpr (std::is_signed_v<InputT> && !std::is_signed_v<OutputT>)
    { // Sadly, std::cmp_* doesn't admit char, so int8_t and uint8_t are excluded.
        return value >= 0 && unsigned(value) <= std::numeric_limits<OutputT>::max();
    }
    if constexpr (!std::is_signed_v<InputT> && std::is_signed_v<OutputT>)
    { // Sadly, std::cmp_* doesn't admit char, so int8_t and uint8_t are excluded.
        return signed(value) <= std::numeric_limits<OutputT>::max() && signed(value) >= 0;
    }
    if constexpr (std::is_signed_v<InputT> == std::is_signed_v<OutputT>)
    {
        return value <= std::numeric_limits<OutputT>::max() && value >= std::numeric_limits<OutputT>::lowest();
    }
}
/// @endcond

/** @name Casts
 \emoji exclamation **Keep in mind that ANY cast should be used ONLY as a last-resort option;
 whenever making types consistent allows to eliminate cast - it is ALWAYS preferred.** \emoji exclamation.
  OTOH, our own specific casts are always **strongly preferred** to generic C++ casts, leaving alone C-style casts.
  @{
*/

/**
 Cast between signed and unsigned representation, or to a narrower type, or both.
**The input type or output type MUST be one of the following unsigned long, long or size\_t.**
 (these are the only types which have different sizes over our current platforms of interest)
 The input type MUST NOT be narrower than the output type.
 The input type and the output type both MUST be integral.

 \emoji exclamation **SHOULD NOT be used outside of cross-platform cast contexts. ** \emoji exclamation.
*/
template <typename OutputT>
constexpr OutputT cross_platform_narrow_cast(auto value)
{
    using InputT = decltype(value);

    static_assert(
        is_any_of_ignoring_cv_v<InputT, long, unsigned long, size_t, std::ptrdiff_t> ||
            is_any_of_ignoring_cv_v<OutputT, long, unsigned long, size_t, std::ptrdiff_t>,
        "cross_platform_narrow_cast: the input type or output type must be one of the following: long, unsigned "
        "long, size_t or ptrdiff_t");
    static_assert(sizeof(OutputT) <= sizeof(InputT),
                  "cross_platform_narrow_cast: the input type should not be narrower than the output type.");
    static_assert(std::is_integral_v<InputT> && std::is_integral_v<OutputT>,
                  "cross_platform_narrow_cast: the input type and the output type should both be integral.\n"
                  "    Consider using precision_cast or round/trunc/floor/ceil_cast.");
    assert(is_representable<OutputT>(value) &&
           "cross_platform_narrow_cast: casting to a type which cannot represent the source value.");
    return static_cast<OutputT>(value);
}

/**
 * \brief Cast between signed and unsigned representation, or to a narrower type, or both.
 
 * The input type and the output type MUST be different.
 * The input type MUST NOT be narrower than the output type.
 * The input type and the output type both MUST be integral.
 * Unnecessary casts are unsupported
 */
template <typename OutputT>
constexpr OutputT narrow_cast(auto value)
{
    using InputT = decltype(value);
    static_assert(!std::is_same_v<std::remove_cv_t<InputT>, std::remove_cv_t<OutputT>> &&
                    (std::is_unsigned_v<InputT> ^ std::is_unsigned_v<OutputT> ||
                     sizeof(OutputT) != sizeof(InputT)),
                  "narrow_cast: the input type and the output type should be different.");
    static_assert(sizeof(OutputT) <= sizeof(InputT),
                  "narrow_cast: the input type should not be narrower than the output type.");
    static_assert(std::is_integral_v<InputT> && std::is_integral_v<OutputT>,
                  "narrow_cast: the input type and the output type should both be integral.\n"
                  "    Consider using precision_cast or round/trunc/floor/ceil_cast.");
    assert(is_representable<OutputT>(value) &&
           "narrow_cast: casting to a type which cannot represent the source value.");
    return static_cast<OutputT>(value);
}

/**
 * \brief Cast from integral to float, or to a narrower floating point representation.
 
 * The output type MUST be a floating point representation.
 * The input type MUST be arithmetic and the output type MUST be a floating point representation.
 * The input type MUST NOT be narrower than the output type.
 */
template <typename OutputT>
constexpr OutputT precision_cast(auto value)
{
    using InputT = decltype(value);
    static_assert(!std::is_same_v<std::remove_cv<OutputT>, std::remove_cv<InputT>>,
                  "precision_cast: the input type should be different from the output type");
    static_assert(!std::is_integral_v<OutputT>,
                  "precision_cast: the output type should be a floating point representation.\n"
                  "    Use round/trunc/floor/ceil_cast instead.");
    static_assert(std::is_arithmetic_v<InputT> && std::is_floating_point_v<OutputT> &&
                      sizeof(OutputT) <= sizeof(InputT),
                  "precision_cast: the input type should be arithmetic and the output type should be a floating point "
                  "representation.\n"
                  "    The input type should not be narrower than the output type.");
    assert(is_representable<OutputT>(value) && "Casting to type which cannot represent the requested value");
    return static_cast<OutputT>(value);
}

/**
 * \brief Cast from float to integral via rounding.
 
 * The input type MUST be a floating point representation.
 * The output type MUST be an integral representation.
 */
template <typename OutputT>
constexpr OutputT round_cast(auto value)
{
    using InputT = decltype(value);
    static_assert(std::is_floating_point_v<InputT>,
                  "round_cast: the input type should be a floating point representation.");
    static_assert(std::is_integral_v<OutputT>, "round_cast: the output type should be an integral representation.");
    assert(is_representable<OutputT>(value) &&
           "round_cast: casting to type which cannot represent the requested value");
    if constexpr (sizeof(OutputT) <= sizeof(long))
        return static_cast<OutputT>(std::lround(value));
    else
        return static_cast<OutputT>(std::llround(value));
}

/**
 * \brief Cast from float to integral via truncating (towards zero).
 
 * The input type MUST be a floating point representation.
 * The output type MUST be an integral representation.
 */
template <typename OutputT>
constexpr OutputT trunc_cast(auto value)
{
    using InputT = decltype(value);
    static_assert(std::is_floating_point_v<InputT>,
                  "trunc_cast: the input type should be a floating point representation.");
    static_assert(std::is_integral_v<OutputT>, "trunc_cast: the output type should be an integral representation.");
    assert(is_representable<OutputT>(value) &&
           "trunc_cast: casting to type which cannot represent the requested value");
    return round_cast<OutputT>(trunc(value));
}

/**
 * \brief Cast from float to integral via floor (towards -INF).
 
 * The input type MUST be a floating point representation.
 * The output type MUST be an integral representation.
 */
template <typename OutputT>
constexpr OutputT floor_cast(auto value)
{
    using InputT = decltype(value);
    static_assert(std::is_floating_point_v<InputT>,
                  "floor_cast: the input type should be a floating point representation.");
    static_assert(std::is_integral_v<OutputT>, "floor_cast: the output type should be an integral representation.");
    assert(is_representable<OutputT>(value) &&
           "floor_cast: casting to type which cannot represent the requested value");
    return round_cast<OutputT>(floor(value));
}

/**
 * \brief Cast from float to integral via ceil (towards +INF).
 
 * The input type MUST be a floating point representation.
 * The output type MUST be an integral representation.
 */
template <typename OutputT>
constexpr OutputT ceil_cast(auto value)
{
    using InputT = decltype(value);
    static_assert(std::is_floating_point_v<InputT>,
                  "ceil_cast: the input type should be a floating point representation.");
    static_assert(std::is_integral_v<OutputT>, "ceil_cast: the output type should be an integral representation.");
    assert(is_representable<OutputT>(value) && "ceil_cast: casting to type which cannot represent the requested value");
    return round_cast<OutputT>(ceil(value));
}

/**
  \brief TODO: to be replaced with our own version based on sixit::lwa::bit_cast<>
 
  suggested differences from sixit::lwa::bit_cast<> (and from std::bit_cast<>):
  - ONLY same size
  - EXPLICIT PROHIBITION on casting pointers to pointers (it is a common mistake to think that simple replacement of reinterpret_cast to bit_cast will magically
      make things better)
  - that's it; non-arithmetic types should also be ok
*/
template <class T, class U>
T bit_cast(const U& u)
{
    // mb: for now very restrictive
    //  only on arithmetic types and
    //  only between types of the same size
    static_assert(sizeof(T) == sizeof(U));
    static_assert(std::is_arithmetic<T>::value);
    static_assert(std::is_arithmetic<U>::value);

    T t;
    memcpy(&t, &u, sizeof(T));
    return t;
}

/*
 * 32-bit versions of the non-member size, ssize functions
 * The standard implementation will take into account non-standard return types
 * of the size function. This can be addressed should it arises.
 * GD 2021-02-26
 */
template <class C>
constexpr uint32_t size32(const C& c)
{
    return cross_platform_narrow_cast<uint32_t>(c.size());
}

template <class C>
constexpr int32_t ssize32(const C& c)
{
    return narrow_cast<int32_t>(c.size());
}

template <class T, std::size_t N>
constexpr uint32_t size32(const T (&array)[N]) noexcept
{
    return cross_platform_narrow_cast<uint32_t>(N);
}

template <class T, std::ptrdiff_t N>
constexpr int32_t ssize32(const T (&array)[N]) noexcept
{
    return cross_platform_narrow_cast<int32_t>(N);
}

/** \brief TODO: rename into type_enum (or maybe into type_list?), also member functions to snake_case
   
 a templatized collection of types - with an ability to search in it, and iterate through it
 */
template <typename... Types>
struct TypeEnum
{
    static constexpr std::size_t size()
    {
        return sizeof...(Types);
    }
    using IndexSequnce = std::index_sequence_for<Types...>;

    template <typename Visitor>
    static constexpr bool forEach(Visitor&& visitor)
    {
        /* Returns true if functor was called on whole collection */
        return forEach(std::forward<Visitor>(visitor), IndexSequnce{});
    }

    template <typename U>
    static constexpr int indexOf()
    {
        return indexOf<U>(IndexSequnce{});
    }

  protected://TODO: change into private
    template <typename U, std::size_t... Indices>
    static constexpr int indexOf(std::index_sequence<Indices...> seq)
    {
        constexpr std::size_t count = (0 + ... + (std::is_same_v<U, Types> ? 1 : 0));
        static_assert(count > 0, "type does not appear in tuple");
        static_assert(count < 2, "type appears more than once in tuple");
        int index; // we have checked that call is valid, no need to initialize
        forEach(
            [&]<typename Elem, int Index>() {
                if constexpr (std::is_same_v<U, Elem>)
                {
                    index = Index;
                    return false; // stop iteration
                }
                return true;
            },
            seq);
        return index;
    }

    template <typename Visitor, std::size_t... Indices>
    static constexpr auto forEach(Visitor&& f, std::index_sequence<Indices...>)
    {
        return (true && ... && f.template operator()<Types, Indices>());
    }
};

template<typename UnsignedIntegralT>
inline constexpr bool is_power_of_two(UnsignedIntegralT n) noexcept
{
    return std::popcount(n) == 1; // fallback: return n != 0 && (n & (n - 1)) == 0;
}

template<typename T>
std::string type_display_name()
{
    // full implementation with demangling yet pending (see TR-1379)
    return typeid(T).name();
}

template<typename T>
std::string type_display_name([[maybe_unused]] T&& t)
{
    return type_display_name<T>();
}

namespace templates {
// constexpr_lambda_returning_vector_to_constexpr_array(f): 
// expect `f() -> constexpr_minimal_vector` lambda, returns std::array.
// 'f' should be a callable object with `constexpr operator()`, because the compiler
// can't prove that 'f' parameter is always a compile-time constant.
template <typename Lambda>
consteval auto constexpr_lambda_returning_vector_to_constexpr_array(Lambda f) noexcept
{
    // clang is extreemely picky here, ensure clang compatibility when modifying code below
    using value_type = std::decay_t<typename decltype(f())::value_type>;
    constexpr size_t size = f().size();
    std::array<value_type, size> result;
    sixit::lwa::copy_n(f().data(), f().size(), result.begin());
    return result;
}
} // namespace templates

} // namespace sixit::guidelines

#endif //sixit_core_guidelines_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Guy Davidson, Dmytro Ivanchykhin, Marcos Bracco, Victor Istomin, Vladyslav Merais

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
