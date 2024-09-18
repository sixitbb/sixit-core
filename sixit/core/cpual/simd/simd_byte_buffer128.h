/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherri Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_core_cpual_simd_simd_byte_buffer128_h_included
#define sixit_core_cpual_simd_simd_byte_buffer128_h_included

#ifdef __SSE2__
#include <emmintrin.h>

#ifdef __SSSE3__
#   include <pmmintrin.h>
#   include <tmmintrin.h>
#endif // __SSSE3__

#include <assert.h>

#include "sixit/core/guidelines.h"

class simd_buffer128 {
  private:
  __m128i data;
  int begin = 0;
  int end = 0;
  
  public:
  constexpr static int max_bytes = 16;
  using plain_type = __m128i;
  using mask_type = int32_t;

  int n_left() const { return end - begin; }
  
  bool fill_from(const plain_type& p) {
    if(begin!=end) { return true; }
    else 
    {
        data = p;
        begin = 0;
        end = 16;
        return false; 
    }
  }

  bool wanna_fill() const {
    return (end - begin) < 8;
  }

  void fill_from(simd_buffer128& other) {
      uint8_t a[8];
      uint8_t b[8];
      uint8_t c[8] = {0, 0, 0, 0, 0, 0, 0, 0};

      _mm_store_si128(reinterpret_cast<__m128i*>(a), data);
      _mm_store_si128(reinterpret_cast<__m128i*>(b), other.data);

      end -= begin;
      memcpy(c, a + begin, end * sizeof(uint8_t));
      begin = 0;

      int add_size = (max_bytes - end) < (other.end - other.begin)
          ? (max_bytes - end)
          : (other.end - other.begin);

      memcpy(c + end, b + other.begin, add_size * sizeof(uint8_t));
      end += add_size;

      _mm_store_si128(&data, *reinterpret_cast<__m128i*>(c));
  }

  void store(uint8_t* p, int n, int overwrite_size) {
    if (n + overwrite_size >= 16)
    {
        _mm_store_si128(reinterpret_cast<__m128i*>(p), data);
        return;
    }

    if constexpr (false)
    {
    
        __m128i tmp_data = data;
        if (n >= 8) 
        {
            *reinterpret_cast<int64_t*>(p) = _mm_cvtsi128_si64(tmp_data);
            p += 8; n -= 8;
            tmp_data = _mm_shuffle_epi32(tmp_data, 78);
        }

        if (n >= 4)
        {
            *reinterpret_cast<int32_t*>(p) = _mm_cvtsi128_si32(tmp_data);
            p += 4; n -= 4;
            tmp_data = _mm_shuffle_epi32(tmp_data, 57);
        }
        int32_t result_data = _mm_cvtsi128_si32(tmp_data);
        if (n) {n--; *(p++) = sixit::guidelines::narrow_cast<uint8_t>(result_data); result_data >>= 8;}
        if (n) {n--; *(p++) = sixit::guidelines::narrow_cast<uint8_t>(result_data); result_data >>= 8;}
        if (n) {n--; *(p++) = sixit::guidelines::narrow_cast<uint8_t>(result_data);}

    }
    else 
    {
        uint8_t storage[16];
        _mm_store_si128(reinterpret_cast<__m128i*>(storage), data);
        memcpy(p, storage, n * sizeof(uint8_t));
    }
  }

  int consume(int n) {
    assert(begin + n <= end);
    begin += n;
    return end - begin;
  }
    
  template<int idx>
  uint8_t get() const {
    assert(begin == 0);
    if constexpr (idx < 4) return uint8_t(_mm_cvtsi128_si32(data) >> idx);
    else if constexpr (idx < 8) return uint8_t(_mm_cvtsi128_si64(data) >> idx);
    else 
    {

      return _mm_extract_epi16(data, idx >> 1) >> ((idx & 1) << 3);

        // uint8_t storage[16] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
        // _mm_store_si128(reinterpret_cast<__m128i*>(storage), data);
        // return storage[idx];
    }
  }

  uint8_t get(int idx) const {
    if constexpr (true)
    {
      idx += begin;
      assert(idx < end);
      uint8_t storage[16] = {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};
      _mm_store_si128(reinterpret_cast<__m128i*>(storage), data);
      return storage[idx];
    } else 
    {
      idx += begin;

      __m128i dst = idx < 8 ? data : _mm_shuffle_epi32(data, 78);
      idx &= ~8;
      idx <<= 3;

      return sixit::guidelines::narrow_cast<uint8_t>(_mm_cvtsi128_si64(dst) >> idx);
    }
  }

  template <uint8_t b>
  mask_type sub()
  {
    __m128i comp_mask = _mm_cmplt_epi8(data, _mm_set1_epi8(b));
    data = _mm_sub_epi8(data, _mm_set1_epi8(b));
    return _mm_movemask_epi8(comp_mask) >> begin;
  }

  template <uint8_t b>
  mask_type equal_to() const {
    __m128i comp_mask = _mm_cmpeq_epi8(data, _mm_set1_epi8(b));
    return _mm_movemask_epi8(comp_mask) >> begin;
  }

  template <uint8_t b>
  mask_type lt_than() const {
    __m128i comp_mask = _mm_cmplt_epi8(data, _mm_set1_epi8(b));
    return _mm_movemask_epi8(comp_mask) >> begin;
  }

  void erase_and_shift_left(int idx) {
    idx += begin;
    uint64_t tmp_storage[2];

    _mm_store_si128(reinterpret_cast<__m128i*>(tmp_storage), data);
    uint64_t *processed_part = tmp_storage + (idx > 7);
    idx &= ~8;
    idx = (idx + 1) << 3;

    uint64_t u2 = (UINT64_C(1) << idx) - 1;
    uint64_t u1 = u2 >> 8;
    *processed_part = ((*processed_part) & ~u2) | (((*processed_part) & u1) << 8);

    if (processed_part != tmp_storage)
    {
      *processed_part = (*processed_part) | ((*tmp_storage) >> 56);
      *tmp_storage <<= 8;
    }
    
    _mm_store_si128(&data, *reinterpret_cast<__m128i*>(tmp_storage));   
    ++begin; 
  }


  uint64_t atoi(int length) const {
    __m128i calculation_data = _mm_sub_epi8(data, _mm_set1_epi8('0'));
    
    int b = begin;
    if (b || length < 16)
    {
        uint64_t tmp_value[2];
        _mm_store_si128(reinterpret_cast<__m128i*>(tmp_value), calculation_data);

        if (b > 7) {
            b -= 8;
            tmp_value[0] = tmp_value[1];
            tmp_value[1] = 0;
        }

        b <<= 3;
        tmp_value[0] = (tmp_value[0] >> b) | (b ? (tmp_value[1] << (64 - b)) : 0);
        tmp_value[1] >>= b;


        if (length < 9) {
            length += 8;
            tmp_value[1] = tmp_value[0];
            tmp_value[0] = 0;
        }

        length = (16 - length) << 3;
        tmp_value[1] = (tmp_value[1] << length) | (length ? (tmp_value[0] >> (64 - length)) : 0);
        tmp_value[0] <<= length;
        _mm_store_si128(&calculation_data, *reinterpret_cast<__m128i*>(tmp_value));
    }

    
//  high * 10 + low
    __m128i tmp_storage;   
#ifdef __SSSE3__
    const __m128i MUL_1_8BIT = _mm_set_epi8(1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10);
    calculation_data = _mm_maddubs_epi16(calculation_data, MUL_1_8BIT);
#else
    const __m128i MUL_1_8BIT = _mm_set_epi8(0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10);
    const __m128i MASK_8 = _mm_set_epi32(0xff00ff, 0xff00ff, 0xff00ff, 0xff00ff);

    tmp_storage = _mm_mullo_epi16(calculation_data, MUL_1_8BIT);
    calculation_data = _mm_add_epi16(tmp_storage, _mm_srli_epi16(calculation_data, 8));
    calculation_data = _mm_and_si128(calculation_data, MASK_8);
#endif // __SSSE3__

//  high * 100 + low
#ifdef __SSSE3__
    const __m128i MUL_2_16BIT = _mm_set_epi16(1, 100, 1, 100, 1, 100, 1, 100);
    calculation_data = _mm_madd_epi16(calculation_data, MUL_2_16BIT);
#else
    const __m128i MUL_2_16BIT = _mm_set_epi16(0, 100, 0, 100, 0, 100, 0, 100);
    const __m128i MASK_16 = _mm_set_epi32(0xffff, 0xffff, 0xffff, 0xffff);

    tmp_storage = _mm_mullo_epi16(calculation_data, MUL_2_16BIT);
    calculation_data = _mm_add_epi32(tmp_storage, _mm_srli_epi32(calculation_data, 16));
    calculation_data = _mm_and_si128(calculation_data, MASK_16);
#endif // __SSSE3___
//  high * 1 0000 + low
    const __m128i MASK_32 = _mm_set_epi32(0, 0xffffffff, 0, 0xffffffff);
    const __m128i MUL_4_32BIT = _mm_set_epi32(0, 10000, 0, 10000);

    tmp_storage = _mm_mul_epu32(calculation_data, MUL_4_32BIT);
    calculation_data = _mm_add_epi64(tmp_storage, _mm_srli_epi64(calculation_data, 32));
    calculation_data = _mm_and_si128(MASK_32, calculation_data);
//  high * 1 0000 0000 + low
    const __m128i MUL_8_64BIT = _mm_set_epi32(0, 0, 0, 100000000);

    tmp_storage = _mm_mul_epu32(calculation_data, MUL_8_64BIT);
    
    calculation_data = _mm_shuffle_epi32(calculation_data, 78);
    calculation_data = _mm_add_epi64(tmp_storage, calculation_data);
    return _mm_cvtsi128_si64(calculation_data);
  }
};
#endif

#endif //sixit_core_cpual_simd_simd_byte_buffer128_h_included

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