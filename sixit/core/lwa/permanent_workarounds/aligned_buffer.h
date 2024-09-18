/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Victor Istomin
*/

/**
 * template <size_t Alignment> class aligned_buffer
 *
 * RAII wrapper for aligned memory allocation.
 * noexcept movable, non-copyable.
 *
 * Enforces alignment/size checks to make sure the buffer will be aligned
 * according to common API requirements.
 */

#ifndef sixit_core_lwa_permanent_workarounds_aligned_buffer_h_included
#define sixit_core_lwa_permanent_workarounds_aligned_buffer_h_included

#include <utility>

#include "sixit/core/core.h"
#include "sixit/core/guidelines.h"

 /*
  * Cumbersome condition: both clang and gcc support std::aligned_alloc, except
  * Android NDK prior to 28 API Level.
  *
  * .../android-ndk-r23c/.../sysroot/usr/include/stdlib.h:
  *   #if __ANDROID_API__ >= 28
  *     void* aligned_alloc(size_t __alignment, size_t __size)
  *   #endif
  */

#if defined(SIXIT_COMPILER_MSVC)
#  include <cstdlib>
#elif defined(SIXIT_OS_ANDROID)
#  include <malloc.h>
#else
#  include <cstdlib>
#endif // defined(SIXIT_COMPILER_MSVC)


namespace sixit::lwa::workarounds
{


    /**
     * RAII wrapper for aligned memory allocation.
     * noexcept movable, non-copyable.
     *
     * Enforces alignment/size checks to make sure the buffer will be aligned
     * according to common API requirements.
     */
    template <size_t Alignment = 16>
    class aligned_buffer
    {
    public:
        static_assert(sixit::guidelines::is_power_of_two(Alignment), "Alignment must be a power of two - memalign() and "
            "_aligned_malloc() requirement");
        static_assert((Alignment % sizeof(void*)) == 0, "it's up to implementation to decide whether to support "
            "granularity less than sizeof(void*) - keep it portable");

        explicit aligned_buffer(size_t size) : ptr_(nullptr)
        {
            /*
             * std::aligned_alloc() _may_ use the posix_memalign() with
             * implementation-defined handling of zero size: may or may not return
             * nullptr. _aligned_malloc() state that it returns nullptr for zero size,
             * but it does not, as of Windows UCRT.
             *
             * Let's play it safe and portable: explicitly define the behavior -
             * zero-sized buffer is nullptr.
             */
            if (size_t alignedSize = align_size(size); alignedSize > 0)
            {
#if defined(SIXIT_COMPILER_MSVC)
                ptr_ = _aligned_malloc(alignedSize, Alignment);
#elif defined(SIXIT_OS_ANDROID)
                ptr_ = memalign(Alignment, alignedSize);
#else
                ptr_ = std::aligned_alloc(Alignment, alignedSize);
#endif
            }
        }

        ~aligned_buffer()
        {
            aligned_free(ptr_);
            ptr_ = nullptr; // easily catch use-after-free
        }

        aligned_buffer() = default;
        aligned_buffer(const aligned_buffer&) = delete;
        aligned_buffer& operator=(const aligned_buffer&) = delete;

        aligned_buffer(aligned_buffer&& temporary) noexcept : ptr_(std::exchange(temporary.ptr_, nullptr))
        {
        }

        aligned_buffer& operator=(aligned_buffer&& temporary) noexcept
        {
            if (this != &temporary)
            {
                aligned_free(ptr_);
                ptr_ = std::exchange(temporary.ptr_, nullptr);
            }

            return *this;
        }

        void* get() const
        {
            return ptr_;
        }

    private:
        void* ptr_ = nullptr;

        static constexpr std::size_t align_size(std::size_t sz)
        {
            return (sz + Alignment - 1) & (~(Alignment - 1));
        }

        static void aligned_free(void* ptr)
        {
#if defined(SIXIT_COMPILER_MSVC)
            _aligned_free(ptr);
#else
            std::free(ptr);
#endif
        }
    };

} // namespace sixit::lwa::workarounds

#endif //sixit_core_lwa_permanent_workarounds_aligned_buffer_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Victor Istomin

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
