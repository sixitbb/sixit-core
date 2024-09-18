/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin
*/

/** \file lwa.h
\brief **sixit/lwa.h**: LWA stands for 'Legacy WorkArounds', the aim is to provide temporary replacements for those language/library features, which are not provided by not-so-recent compilers.
 
 One IMPORTANT requirement is that these replacements are indeed _temporary_: as soon as we see that the feature is officially supported (using respective feature macro) - we fall back to it.
 
 In addition, we ensure that there is an obvious 1:1 correspondence between 'legacy' (LWA) syntax and standard C++ constructs.
 */

#ifndef sixit_core_lwa_h_included
#define sixit_core_lwa_h_included

#include "sixit/core/core.h"

#include <cstdint>
#include <cstring>
static_assert( sizeof(uint32_t) == 4 ); // for avoidance of any doubt

#if defined(__has_include) && __has_include(<version>) 
#   include <version>   // for feature-test macros
#endif

#if (defined __cpp_lib_bit_cast) || (defined __cpp_lib_endian)
#  include <bit>
#endif // __cpp_lib_bit_cast || __cpp_lib_endian

#if __cpp_concepts && defined(__has_include) && __has_include(<version>)
#   include <version>
#   if __cpp_lib_concepts
#       define SIXIT_LWA_OPTIONAL_CONCEPT 1
#   endif
#endif // __cpp_concepts && defined(__has_include) && __has_include(<version>)
#if SIXIT_LWA_OPTIONAL_CONCEPT
#   define SIXIT_LWA_OPTIONAL_REQUIRES(Concept, Type) requires Concept<Type>
#   define SIXIT_LWA_OPTIONAL_REQUIRES_FP(Concept, Type, FP) requires Concept<Type, FP>
#   define SIXIT_LWA_OPTIONAL_REQUIRES2(Concept1, Type1, Concept2, Type2) requires Concept1<Type1> && Concept2<Type2>
#else
#   define SIXIT_LWA_OPTIONAL_REQUIRES(Concept, Type)
#   define SIXIT_LWA_OPTIONAL_REQUIRES_FP(Concept, Type, FP)
#   define SIXIT_LWA_OPTIONAL_REQUIRES2(Concept1, Type1, Concept2, Type2)
#endif

#if defined(__has_include) && __has_include(<version>)
#include <version>
#   if __cpp_lib_constexpr_vector && ( !defined(SIXIT_COMPILER_GCC) || (SIXIT_COMPILER_GCC < 1201 || SIXIT_COMPILER_GCC >= 1401) || !defined(_GLIBCXX_DEBUG)) // NOTE: SIXIT_COMPILER_GCC + _GLIBCXX_DEBUG results in __cpp_lib_constexpr_vector being set while vector is not constexpr (until c++2g). Bug report pending
#       define SIXIT_USE_CONSTEXPR_STD_VECTOR 1
#   endif
#   if __cpp_lib_constexpr_algorithms
#       define SIXIT_USE_CONSTEXPR_STD_ALGORITHMS 1
#   endif
#endif

#if defined(SIXIT_USE_CONSTEXPR_STD_VECTOR)
#   include <vector>
#endif

#if defined(SIXIT_USE_CONSTEXPR_STD_ALGORITHMS)
#   include <algorithm>
#endif

#if defined(__has_include) && __has_include(<version>)
#  include <version>
#  if defined( __cpp_lib_format) && defined( __cpp_lib_print)
#   include <format>
#   include <print>
namespace sixit::lwa {
    namespace fmt = ::std;
}
#  else
#    include "lwa/format.h"
#  endif
#else
#  include "lwa/format.h"
#endif

#if defined(__cpp_lib_to_underlying	)
#  include <utility>
namespace sixit::lwa {
    template< class Enum >
    constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return std::to_underlying(e);
    }
}
#else
namespace sixit::lwa {
    template< class Enum >
    constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}
#endif


namespace sixit::lwa {

// bit cast
template<class To, class From>
constexpr To bit_cast(const From& from)
{
#ifdef __cpp_lib_bit_cast
    return std::bit_cast<To, From>( from );
#else
    static_assert(sizeof(To) == sizeof(From));
    static_assert(std::is_trivially_copyable<To>::value);
    static_assert(std::is_trivially_copyable<From>::value);
    
    if constexpr( alignof(From) >= alignof(To) )
    {
        To to;
        memcpy(&to, &from, sizeof(To));
        return to;
    }
    else
    {
        union U
        {
            From ff;
            To tt;
        } u;
        u.ff = from;
        return u.tt;
    }
#endif // __cpp_lib_bit_cast
}

#ifdef __cpp_lib_endian
  using endian = ::std::endian;
#else
  enum class endian
  {
    little = 0x04030201,
    big    = 0x01020304,
    native = std::bit_cast<uint32_t, char[sizeof(uint32_t)]>( {1, 2, 3, 4} ),
  };
#endif // __cpp_lib_endian


// literal helpers
struct string_literal_helper_base {};
template<uint32_t charCnt, typename Char = char>
struct string_literal_helper : public string_literal_helper_base
{
    Char str[charCnt]{};
    constexpr string_literal_helper(const Char (&s)[charCnt])
    {
        std::copy_n(s, charCnt, str);
    }
    template <uint32_t charCnt2>
    constexpr string_literal_helper(const Char (&s1)[charCnt2], const Char (&s2)[charCnt - charCnt2 + 1])
    {
        std::copy_n(s1, charCnt2, str);
        std::copy_n(s2, charCnt - charCnt2 + 1, str + charCnt2 - 1);
    }
    constexpr operator const Char*() const
    {
        return str;
    }
};
template <uint32_t N1, uint32_t N2, typename Char = char>
constexpr string_literal_helper<N1 + N2 - 1, Char> operator+(const string_literal_helper<N1, Char>& s1, const string_literal_helper<N2, Char>& s2)
{
    return string_literal_helper<N1 + N2 - 1, Char>(s1.str, s2.str);
}

template <uint32_t N1, uint32_t N2, typename Char = char>
constexpr string_literal_helper<N1 + N2 - 1, Char> operator+(const string_literal_helper<N1, Char>& s1, const Char (&s2)[N2])
{
    return string_literal_helper<N1 + N2 - 1, Char>(s1.str, s2);
}

template <uint32_t N1, uint32_t N2, typename Char = char>
constexpr string_literal_helper<N1 + N2 - 1, Char> operator+(const Char (&s1)[N1], const string_literal_helper<N2, Char>& s2)
{
    return string_literal_helper<N1 + N2 - 1, Char>(s1, s2.str);
}

template<class T>
static constexpr bool is_string_literal_helper = std::is_base_of_v<string_literal_helper_base, std::decay_t<T>>;

struct floating_const_helper_base {};
template <class T>
class floating_const_helper : public floating_const_helper_base {
public:
    using value_type      = T;    
    constexpr floating_const_helper( T f ) { _Elem = f; }
    constexpr T val() const { return _Elem; }
    T _Elem;
};
template<class T>
static constexpr bool is_floating_const_helper = std::is_base_of_v<floating_const_helper_base, std::decay_t<T>>;

#if defined(SIXIT_USE_CONSTEXPR_STD_ALGORITHMS)
using std::copy_n;
#else
template <class InputIt, class Size, class OutputIt> 
constexpr OutputIt copy_n(InputIt src, Size count, OutputIt dest) noexcept(noexcept(*dest++ = *src++))
{
    // suboptimal algorithm (doesn't matter for constexpt), for relaxed requirements on iterator_category.
    // Optimize once needed.
    for (Size i = 0; i < count; ++i)
    {
        *dest++ = *src++;
    }

    return dest;
}
#endif // SIXIT_USE_CONSTEXPR_STD_ALGORITHMS

#if defined(SIXIT_USE_CONSTEXPR_STD_VECTOR)
// constexpr std::vector has a priority over lwa-workaround
template <typename T>
using constexpr_minimal_vector = std::vector<T>;
#else
// no constexpr std::vector, use minimalistic replacement instead
template <typename T>
class constexpr_minimal_vector
{
  private:
    T* data_ = nullptr; // require T() to be default-constructible, improve once needed
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;

  public:
    using value_type = T;

    constexpr constexpr_minimal_vector() noexcept = default;
    constexpr ~constexpr_minimal_vector() noexcept
    {
        delete[] data_;
    }

    constexpr constexpr_minimal_vector(const constexpr_minimal_vector& other)
    {
        T* new_data = new T[other.size_];
        copy_n(other.data_, other.size_, new_data);
        data_ = new_data;
        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    constexpr constexpr_minimal_vector(constexpr_minimal_vector&& other) noexcept
    {
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    constexpr void reserve(std::size_t new_capacity)
    {
        if (new_capacity <= capacity_)
            return;

        T* new_data = new T[new_capacity];
        copy_n(data_, size_, new_data);
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

    constexpr void push_back(const T& value)
    {
        if (size_ == capacity_)
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);

        data_[size_] = value;
        ++size_;
    }

    constexpr const T* data() const noexcept
    {
        return data_;
    }
    constexpr const T& operator[](std::size_t i) const noexcept
    {
        return data_[i];
    }
    constexpr T& operator[](std::size_t i) noexcept
    {
        return data_[i];
    }
    constexpr std::size_t size() const noexcept
    {
        return size_;
    }
    constexpr std::size_t capacity() const noexcept
    {
        return capacity_;
    }
};
#endif // SIXIT_USE_CONSTEXPR_STD_VECTOR

#ifdef __has_cpp_attribute
#  if __has_cpp_attribute(assume)
#    define SIXIT_LWA_ASSUME(expr) [[assume(expr)]]
#  endif
#endif

#ifndef SIXIT_LWA_ASSUME
#  if defined(SIXIT_COMPILER_ANY_CLANG)
#    define SIXIT_LWA_ASSUME(expr) __builtin_assume(expr)
#  elif defined(SIXIT_COMPILER_GCC)
#    define SIXIT_LWA_ASSUME(expr) if (expr) {} else { __builtin_unreachable(); }
#  elif defined(SIXIT_COMPILER_MSVC)
#    define SIXIT_LWA_ASSUME(expr) __assume(expr)
#  endif
#endif

#ifdef __cpp_lib_unreachable
#    define SIXIT_LWA_ASSUME_UNREACHABLE std::unreachable()
#endif

#ifndef SIXIT_LWA_ASSUME_UNREACHABLE
#  if defined(SIXIT_COMPILER_ANY_CLANG)
#    define SIXIT_LWA_ASSUME_UNREACHABLE __builtin_assume(0)
#  elif defined(SIXIT_COMPILER_GCC)
#    define SIXIT_LWA_ASSUME_UNREACHABLE __builtin_unreachable()
#  elif defined(SIXIT_COMPILER_MSVC)
#    define SIXIT_LWA_ASSUME_UNREACHABLE __assume(0)
#  endif
#endif


#ifdef __cpp_size_t_suffix
#  define SIXIT_LWA_Z(x) x##z
#else
#  define SIXIT_LWA_Z(x) static_cast<std::ptrdiff_t>(x)
#endif

#ifdef __cpp_lib_ssize
template<typename ContainerT>
std::ptrdiff_t ssize(const ContainerT& c) {
    return std::ssize(c);
}
#else
template<typename ContainerT>
std::ptrdiff_t ssize(const ContainerT& c) {
    return static_cast<std::ptrdiff_t>(c.size());
}
#endif


} // sixit::lwa



#endif //sixit_core_lwa_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin

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
