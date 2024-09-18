/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherri Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_core_cpual_simd_simd_byte_buffer512_h_included
#define sixit_core_cpual_simd_simd_byte_buffer512_h_included

#if defined(__AVX__) && defined(__AVX2__) && defined(__SSE2__)

#include <emmintrin.h>
#   include <immintrin.h>
#include <stdint.h>
#include <cstring>

class simd_buffer512 {
private:
    __m512i data;
    int begin = 0;
    int end = 0;

public:
    constexpr static int max_bytes = 64;
    using plain_type = __m512i;
    using mask_type = uint64_t;

    int n_left() const { return end - begin; }

    bool fill_from(const plain_type& p) {
        if (begin != end) { return true; }
        else
        {
            data = p;
            begin = 0;
            end = 64;
            return false;
        }
    }

    bool wanna_fill() const {
        return (end - begin) < 32;
    }

    void fill_from(simd_buffer512& other)
    {
        uint8_t a[64];
        uint8_t b[64];
        uint8_t c[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        _mm512_store_si512(reinterpret_cast<__m512i*>(a), data);
        _mm512_store_si512(reinterpret_cast<__m512i*>(b), other.data);

        end -= begin;
        memcpy(c, a + begin, end * sizeof(uint8_t));
        begin = 0;

        int add_size = (max_bytes - end) < (other.end - other.begin)
            ? (max_bytes - end)
            : (other.end - other.begin);

        memcpy(c + end, b + other.begin, add_size * sizeof(uint8_t));
        end += add_size;

        _mm512_store_si512(&data, *reinterpret_cast<__m512i*>(c));
    }

    void store(uint8_t* p, int n, int overwrite_size) {
        if (n + overwrite_size >= 64)
        {
            _mm512_store_si512(reinterpret_cast<__m512i*>(p), data);
            return;
        }
        uint8_t storage[64];
        _mm512_store_si512(reinterpret_cast<__m512i*>(storage), data);
        memcpy(p, storage, n * sizeof(uint8_t));
    }

    int consume(int n) {
        assert(begin + n <= end);
        begin += n;
        return end - begin;
    }

    template<int idx>
    uint8_t get() const {
        assert(begin == 0);
        if constexpr (true)
        {
            __m128i tmp_storage = _mm512_extracti32x4_epi32(data, idx >> 4);

            return _mm_extract_epi16(data, (idx & 15) >> 1) >> ((idx & 1) << 3);
            
            // uint8_t storage[16] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
            // _mm_store_si128(reinterpret_cast<__m128i*>(storage), tmp_storage);
            // return storage[idx & 15];
        }
        else
        {
            uint8_t storage[64] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
            _mm512_store_si512(reinterpret_cast<__m512i*>(storage), data);
            return storage[idx];
        }
    }

    uint8_t get(int idx) const {
        idx += begin;
        uint8_t storage[64] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
        _mm512_store_si512(reinterpret_cast<__m512i*>(storage), data);
        return storage[idx];
    }

    template <uint8_t b>
    mask_type sub()
    {
        int64_t comp_mask = lt_than<b>();
        data = _mm512_sub_epi8(data, _mm512_set1_epi8(b));
        return comp_mask;
    }

    template <uint8_t b>
    mask_type equal_to() const {
        return _mm512_cmpeq_epu8_mask(data, _mm512_set1_epi8(b)) >> begin;
    }

    template <uint8_t b>
    mask_type lt_than() const {
        return _mm512_cmplt_epu8_mask(data, _mm512_set1_epi8(b)) >> begin;
    }

    // void printBits(__m512i a) const {
    //     uint8_t storage[64];
    //     _mm512_store_si512(reinterpret_cast<__m512i*>(storage), a);
    //     for (auto b : storage) {
    //         for (size_t i = 0; i < 8; ++i) {
    //             std::cout << (b & 1);
    //             b >>= 1;
    //         }
    //         std::cout << " ";
    //     }
    //     std::cout << "\n";
    //     std::cout.flush();
    // }

    void erase_and_shift_left(int idx) {
        idx += begin;
        uint64_t storage[8] = { 0,0,0,0,0,0,0,0 };
        _mm512_store_si512(reinterpret_cast<__m512i*>(storage), data);
        

        int small_shift = idx & 7;
        int big_shift = idx >> 3;

        uint64_t mask_low = (UINT64_C(1) << (small_shift * 8)) - 1;
        uint64_t mask_high = ~mask_low << 8;

        storage[big_shift] = ((storage[big_shift] & mask_high) >> 8) | (storage[big_shift] & mask_low);
        while (big_shift)
        {
            storage[big_shift] = (storage[big_shift] << 8) | (storage[big_shift - 1] >> 56);
            --big_shift;
        }
        storage[big_shift] <<= 8;
        ++begin;
        _mm512_store_si512(&data, *reinterpret_cast<__m512i*>(storage));
    }

    // void printBits(__m256i a) const {
    //     uint8_t storage[32];
    //     _mm256_store_si256(reinterpret_cast<__m256i*>(storage), a);
    //     for (auto b : storage) {
    //         for (size_t i = 0; i < 8; ++i) {
    //             std::cout << (b & 1);
    //             b >>= 1;
    //         }
    //         std::cout << " ";
    //     }
    //     std::cout << "\n";
    //     std::cout.flush();
    // }

    uint64_t atoi(int length) const {
        __m512i calculation_data512 = _mm512_subs_epi8(data, _mm512_set1_epi8('0'));
        int b = begin;
        if (b || length < 64)
        {
            uint64_t tmp_value[8];
            _mm512_store_si512(reinterpret_cast<__m512i*>(tmp_value), calculation_data512);

            {
                int big_shift = b >> 3;
                int small_shift = (b % 8) << 3;

                for (int i = 0; i < 8 && big_shift > 0; ++i)
                    tmp_value[i] = i + big_shift < 8 ? tmp_value[i + big_shift] : 0;

                for (int i = 0; i < 8 && small_shift > 0; ++i)
                    tmp_value[i] = tmp_value[i] >> small_shift | ((i < 7 ? tmp_value[i + 1] : 0) << (64 - small_shift));
            }

            {
                int big_shift = (64 - length) >> 3;
                int small_shift = ((64 - length) % 8) << 3;

                for (int i = 7; i >= 0 && big_shift > 0; --i)
                    tmp_value[i] = i - big_shift >= 0 ? tmp_value[i - big_shift] : 0;

                for (int i = 7; i >= 0 && small_shift > 0; --i)
                    tmp_value[i] = tmp_value[i] << small_shift | ((i > 0 ? tmp_value[i - 1] : 0) >> (64 - small_shift));
            }

            _mm512_store_si512(&calculation_data512, *reinterpret_cast<__m512i*>(tmp_value));
        }

        const __m512i MUL_1_8BIT = _mm512_set_epi8(1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10,
            1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10,
            1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10,
            1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10);
        calculation_data512 = _mm512_maddubs_epi16(calculation_data512, MUL_1_8BIT);

        __m256i calculation_data256 = _mm512_cvtepi16_epi8(calculation_data512);

        const __m256i MUL_2_16BIT = _mm256_set_epi8(1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100,
            1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100);
        calculation_data256 = _mm256_maddubs_epi16(calculation_data256, MUL_2_16BIT);

        const __m256i MUL_4_32BIT = _mm256_set_epi16(1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000);
        calculation_data256 = _mm256_madd_epi16(calculation_data256, MUL_4_32BIT);

        const __m256i MUL_8_64BIT = _mm256_set_epi32(0, 100000000, 0, 100000000, 0, 100000000, 0, 100000000);

        const __m256i MASK_32_1 = _mm256_set_epi32(0, 0xffffffff, 0, 0xffffffff, 0, 0xffffffff, 0, 0xffffffff);

        __m256i tmp_data = _mm256_and_si256(calculation_data256, MASK_32_1);
        calculation_data256 = _mm256_srli_epi64(calculation_data256, 32);

        tmp_data = _mm256_mullo_epi64(tmp_data, MUL_8_64BIT);
        calculation_data256 = _mm256_add_epi64(tmp_data, calculation_data256);

        uint64_t result[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(result), calculation_data256);

        return result[2] * 10000000000000000 + result[3];
    }
};

#endif // __AVX__ & __SSE2__ & __AVX2__

#endif //sixit_core_cpual_simd_simd_byte_buffer512_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherri Ignatchenko, Serhii Iliukhin

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
