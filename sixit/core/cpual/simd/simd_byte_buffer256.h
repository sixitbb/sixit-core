/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherri Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_core_cpual_simd_simd_byte_buffer256_h_included
#define sixit_core_cpual_simd_simd_byte_buffer256_h_included

#if defined(__AVX__) && defined(__AVX2__) && defined(__SSE2__)

#include <emmintrin.h>
#   include <immintrin.h>
#include <stdint.h>
#include <cstring>

class simd_buffer256 {
  private:
  __m256i data;
  int begin = 0;
  int end = 0;
  
  public:
    constexpr static int max_bytes = 32;
    using plain_type = __m256i;
    using mask_type = uint32_t;

    int n_left() const { return end - begin; }
  
    bool fill_from(const plain_type& p) {
        if(begin!=end) { return true; }
        else 
        {
            data = p;
            begin = 0;
            end = 32;
            return false; 
        }
    }

    bool wanna_fill() const {
        return (end - begin) < 16;
    }

    void fill_from(simd_buffer256& other) 
    {
        uint8_t a[32];
        uint8_t b[32];
        uint8_t c[32] = {0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0 };

        _mm256_store_si256(reinterpret_cast<__m256i*>(a), data);
        _mm256_store_si256(reinterpret_cast<__m256i*>(b), other.data);

        end -= begin;
        memcpy(c, a + begin, end * sizeof(uint8_t));
        begin = 0;

        int add_size = (max_bytes - end) < (other.end - other.begin)
            ? (max_bytes - end)
            : (other.end - other.begin);

        memcpy(c + end, b + other.begin, add_size * sizeof(uint8_t));
        end += add_size;

        _mm256_store_si256(&data, *reinterpret_cast<__m256i*>(c));
    }

   void store(uint8_t* p, int n, int overwrite_size) {
     if (n + overwrite_size >= 32)
     {
         _mm256_store_si256(reinterpret_cast<__m256i*>(p), data);
         return;
     }
     uint8_t storage[32];
     _mm256_store_si256(reinterpret_cast<__m256i*>(storage), data);
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
         return sixit::guidelines::narrow_cast<uint8_t>(_mm256_extract_epi8(data, idx));
     } else 
     {     
         uint8_t storage[32] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
         _mm256_store_si256(reinterpret_cast<__m256i*>(storage), data);
         return storage[idx];
     }
   }

  uint8_t get(int idx) const {
    idx += begin;
    assert(idx < end);
    uint8_t storage[32];
    _mm256_store_si256(reinterpret_cast<__m256i*>(storage), data);
    return storage[idx];
  }

   template <uint8_t b>
   mask_type sub()
   {
     int32_t comp_mask = lt_than<b>();
     data = _mm256_sub_epi8(data, _mm256_set1_epi8(b));
     return comp_mask;
   }

  template <uint8_t b>
  mask_type equal_to() const {
#if defined(__AVX512VL__) && defined(__AVX512BW__)
    return _mm256_cmpeq_epi8_mask(data, _mm256_set1_epi8(b)) >> begin;
#else
    return _mm256_movemask_epi8(_mm256_cmpeq_epi8(data, _mm256_set1_epi8(b))) >> begin;
#endif   
  }

  template <uint8_t b>
  mask_type lt_than() const {
#if defined(__AVX512VL__) && defined(__AVX512BW__)
    return _mm256_cmplt_epi8_mask(data, _mm256_set1_epi8(b)) >> begin;
#else
    return _mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_set1_epi8(b), data)) >> begin;
#endif
  }
   
  void erase_and_shift_left(int idx) {
     idx += begin;
     uint64_t tmp_storage[4];

     _mm256_store_si256(reinterpret_cast<__m256i*>(tmp_storage), data);
     uint64_t *processed_part = tmp_storage + (idx > 7) + (idx > 15) + (idx > 23);
     idx &= ~24;
     idx = (idx + 1) << 3;

     uint64_t u2 = (UINT64_C(1) << idx) - 1;
     uint64_t u1 = u2 >> 8;
     *processed_part = ((*processed_part) & ~u2) | (((*processed_part) & u1) << 8);
     --processed_part;

     if (processed_part >= tmp_storage)
     {
       *(processed_part + 1) = *(processed_part + 1) | (*processed_part >> 56);
       *processed_part-- <<= 8;
       if (processed_part >= tmp_storage)
       {
         *(processed_part + 1) = *(processed_part + 1) | (*processed_part >> 56);
         *processed_part-- <<= 8;
         if (processed_part >= tmp_storage)
         {
           *(processed_part + 1) = *(processed_part + 1) | (*processed_part >> 56);
           *processed_part-- <<= 8;
         }
       }
     }

     _mm256_store_si256(&data, *reinterpret_cast<__m256i*>(tmp_storage));
     ++begin; 
  }

//   void printBits(__m128i a) const {
//       uint8_t storage[16];
//       _mm_store_si128(reinterpret_cast<__m128i*>(storage), a);
//       for (auto b : storage) {
//           for (size_t i = 0; i < 8; ++i) {
//               std::cout << (b & 1);
//               b >>= 1;
//           }
//           std::cout << " ";
//       }
//         std::cout << "\n";
        
//   }

  uint64_t atoi(int length) const {
       __m256i calculation_data256 = _mm256_sub_epi8(data, _mm256_set1_epi8('0'));
    
     int b = begin;
     if (b || length < 32)
     {
         uint64_t tmp_value[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp_value), calculation_data256);

        if (b > 15)
        {
            if (b > 23) 
            {
                b -= 24;
                tmp_value[0] = tmp_value[3];
                tmp_value[1] = tmp_value[2] = tmp_value[3] = UINT64_C(0);
            }
            else {
                b -= 16;
                tmp_value[0] = tmp_value[2];
                tmp_value[1] = tmp_value[3];
                tmp_value[2] = tmp_value[3] = UINT64_C(0);
            }
        }
        else {
            if (b > 7)
            {
                b -= 8;
                tmp_value[0] = tmp_value[1];
                tmp_value[1] = tmp_value[2];
                tmp_value[2] = tmp_value[3];
                tmp_value[3] = UINT64_C(0);
            }
        }
        
        if (b)
        {
            b <<= 3;
            tmp_value[0] = (tmp_value[0] >> b) | (tmp_value[1] << (64 - b));
            tmp_value[1] = (tmp_value[1] >> b) | (tmp_value[2] << (64 - b));
            tmp_value[2] = (tmp_value[2] >> b) | (tmp_value[3] << (64 - b));
            tmp_value[3] = (tmp_value[3] >> b);
        }

        length = 32 - length;
        if (length > 15)
        {
            if (length > 23)
            {
                length -= 24;
                tmp_value[3] = tmp_value[0];
                tmp_value[2] = tmp_value[1] = tmp_value[0] = 0;
            }
            else {
                length -= 16;
                tmp_value[3] = tmp_value[1];
                tmp_value[2] = tmp_value[0];
                tmp_value[1] = tmp_value[0] = 0;
            }
        }
        else {
            if (length > 7)
            {
                length -= 8;
                tmp_value[3] = tmp_value[2];
                tmp_value[2] = tmp_value[1];
                tmp_value[1] = tmp_value[0];
                tmp_value[0] = 0;
            }
        }

        if (length)
        {
           length <<= 3;
           tmp_value[3] = (tmp_value[3] << length) | (tmp_value[2] >> (64 - length));
           tmp_value[2] = (tmp_value[2] << length) | (tmp_value[1] >> (64 - length));
           tmp_value[1] = (tmp_value[1] << length) | (tmp_value[0] >> (64 - length));
           tmp_value[0] = (tmp_value[0] << length);
        }
        _mm256_store_si256(&calculation_data256, *reinterpret_cast<__m256i*>(tmp_value));
     }

 //  high * 10 + low
     
     const __m256i MUL_1_8BIT = _mm256_set_epi8(1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10,
                                                1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10);
     calculation_data256 = _mm256_maddubs_epi16(calculation_data256, MUL_1_8BIT);

     __m128i calculation_data128;
     [[maybe_unused]] __m128i tmp_storage;

#if defined(__AVX512VL__) && defined(__AVX512BW__)
     calculation_data128 = _mm256_cvtepi16_epi8(calculation_data256);
#else
     __m256i tmp256 = _mm256_shuffle_epi8(calculation_data256, _mm256_setr_epi8(
         0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15,
         0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15));
     
     uint64_t intermediate_storage[4];
     _mm256_store_si256(reinterpret_cast<__m256i*>(intermediate_storage), tmp256);
     intermediate_storage[1] |= intermediate_storage[2];

     _mm_store_si128(&calculation_data128, *reinterpret_cast<__m128i*>(intermediate_storage));
#endif
          
  //  high * 100 + low
 #ifdef __SSSE3__
     //printBits(calculation_data128);
     const __m128i MUL_2_16BIT = _mm_set_epi8(1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100);
     calculation_data128 = _mm_maddubs_epi16(calculation_data128, MUL_2_16BIT);
     //printBits(calculation_data128);
 #else
     const __m128i MUL_2_16BIT = _mm_set_epi8(0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100, 0, 100);
     const __m128i MASK_16_1 = _mm_set_epi32(0x00ff00ff, 0x00ff00ff, 0x00ff00ff, 0x00ff00ff);
     const __m128i MASK_16_2 = _mm_set_epi32(0xff00ff00, 0xff00ff00, 0xff00ff00, 0xff00ff00);  

     tmp_storage = _mm_and_si128(calculation_data128, MASK_16_1);
     calculation_data128 = _mm_and_si128(calculation_data128, MASK_16_2);

     tmp_storage = _mm_mullo_epi16(tmp_storage, MUL_2_16BIT);
     calculation_data128 = _mm_add_epi32(tmp_storage, _mm_srli_epi32(calculation_data128, 8));
 #endif // __SSSE3___

 //  high * 1 0000 + low
#ifdef __SSSE3__
     const __m128i MUL_4_32BIT = _mm_set_epi16(1, 10000, 1, 10000, 1, 10000, 1, 10000);
     calculation_data128 = _mm_madd_epi16(calculation_data128, MUL_4_32BIT);
#else
     const __m128i MUL_4_32BIT = _mm_set_epi16(0, 10000, 0, 10000, 0, 10000, 0, 10000);
     tmp_storage = _mm_add_epi32(_mm_mullo_epi16(calculation_data128, MUL_4_32BIT),
                                 _mm_slli_epi32(_mm_mulhi_epi16(calculation_data128, MUL_4_32BIT), 16));
     calculation_data128 = _mm_add_epi64(tmp_storage, _mm_srli_epi32(calculation_data128, 16));
#endif
 //  high * 1 0000 0000 + low
     const __m128i MUL_8_64BIT = _mm_set_epi32(0, 100000000, 0, 100000000);
     tmp_storage = _mm_mul_epu32(calculation_data128, MUL_8_64BIT);
     calculation_data128 = _mm_add_epi64(tmp_storage, _mm_srli_epi64(calculation_data128, 32));
     
     uint64_t result[2];
     _mm_store_si128(reinterpret_cast<__m128i*>(result), calculation_data128);
     return result[0] * UINT64_C(10000'0000'0000'0000) + result[1];
   }
};

#endif // __AVX__ & __SSE2__ & __AVX2__

#endif //sixit_core_cpual_simd_simd_byte_buffer256_h_included

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