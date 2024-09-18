/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherri Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_core_cpual_simd_simd_byte_buffer64_h_included
#define sixit_core_cpual_simd_simd_byte_buffer64_h_included

#include <stdint.h>
#include <bit>
#include <assert.h>

class simd_buffer64 {
private:
  uint64_t data = UINT64_C(0x8000000000000000);
  int n_bytes = 0;
  constexpr static uint64_t guard_mask_low = ( UINT64_C(1) << 0 ) | ( UINT64_C(1) << 9 ) | ( UINT64_C(1) << 18 ) | ( UINT64_C(1) << 27 ) | 
                                             ( UINT64_C(1) << 36 ) | ( UINT64_C(1) << 45 ) | ( UINT64_C(1) << 54 );
  constexpr static uint64_t guard_mask = guard_mask_low | ( UINT64_C(1) << 63 );
  constexpr static uint64_t neg_guard_mask = ~guard_mask;
  template<uint8_t b>
  constexpr static uint64_t repeat() {
    return ( uint64_t(b) << 1 ) | ( uint64_t(b) << 10 ) | ( uint64_t(b) << 19 ) | ( uint64_t(b) << 28 ) | ( uint64_t(b) << 37 ) | ( uint64_t(b) << 46 ) | ( uint64_t(b) << 55 );
  }
  
public:
  constexpr static int max_bytes = 7;
  using plain_type = uint64_t;
  class mask_type { 
    private:
    uint64_t m;
    friend class simd_buffer64;
    mask_type(uint64_t m_) : m(m_) {
    }
    
    public:
    int popcount() const {
      return std::popcount(m);
    }
    int countl_zero() const {
      assert((m == 0 || ((std::countl_zero(m)) % 9 ) == 0 ));
      return ((std::countl_zero(m)) / 9);
    }
    mask_type operator |(const mask_type& other) {
      return mask_type( m | other.m );
    }
    mask_type operator &(const mask_type& other) {
      return mask_type( m & other.m );
    }
    mask_type operator<<(int idx) {
      idx += idx << 3;
      return mask_type(m << idx);
    }
    mask_type operator~() {
      return mask_type(~m & simd_buffer64::guard_mask);
    }
  };
  
  int n_left() const { return n_bytes; }

  template<int idx>
  uint8_t get() const {
    static_assert(idx >= 0 && idx < max_bytes);
    return uint8_t(data >> (1 + (max_bytes - idx - 1) * 9));
  }

  uint8_t get(int idx) const {
    assert(idx >= 0 && idx < max_bytes);
    return uint8_t(data >> (1 + (max_bytes - idx - 1) * 9));
  }
  
  int fill_from(const uint64_t& u, int n_available) {
    int not_readed = n_available - max_bytes + n_bytes;
    not_readed = not_readed > 0 ? not_readed : 0;
    n_available -= not_readed;

    int shift = 9 * (max_bytes - n_available);
    n_bytes += n_available - not_readed;

    uint64_t reverted = guard_mask;
    reverted |= ((u & UINT64_C(0xff)) << 55);
    reverted |= ((u & UINT64_C(0xff00)) << 38);
    reverted |= ((u & UINT64_C(0xff0000)) << 21);
      reverted |= ((u & UINT64_C(0xff000000)) << 4);
    if (n_available > 4) {
      reverted |= ((u & UINT64_C(0xff00000000)) >> 13);
      reverted |= ((u & UINT64_C(0xff0000000000)) >> 30);
      reverted |= ((u & UINT64_C(0xff000000000000)) >> 47);
    }
    data |= reverted << shift;
    
    return not_readed;
  }

  void store(uint8_t* p, int n) {
    for(int i = 0; i < n; ++i, ++p)
      *p = get(i);
  }

  int consume(int n) {
    assert(n <= n_bytes);
    data <<= (n * 9);
    return n_bytes -= n;
  }

  template <uint8_t b>
  mask_type equal_to() const {
    static_assert(b >= 0 && b < 255);
    constexpr static uint64_t b_data = repeat<b>();
    uint64_t not_borrowed1 = data - (b_data << (9 * (max_bytes - n_bytes)));
    constexpr static uint64_t b_data2 = repeat<b + 1>();
    uint64_t not_borrowed2 = data - (b_data2 << (9 * (max_bytes - n_bytes)));

    uint64_t eq = not_borrowed1 & ~not_borrowed2;
    return mask_type((eq & guard_mask));
  }

  template <uint8_t b>
  mask_type lt_than() const {
    constexpr static uint64_t b_data = repeat<b>();
    uint64_t not_borrowed = data - (b_data << (9 * (max_bytes - n_bytes)));

    uint64_t lt = not_borrowed;
    return mask_type((lt & guard_mask));
  }

  template <uint8_t b>
  mask_type sub() {
    constexpr static uint64_t b_data = repeat<b>();
    data -= b_data << (9 * (max_bytes - n_bytes));

    uint64_t lt = ~data;
    return mask_type((lt & guard_mask));
  }

  void erase_and_shift_left(int idx) {
    assert(idx>=0 && idx < n_bytes);
    uint64_t u2 = (UINT64_C(1) << ((max_bytes - idx) * 9)) - 1;
    uint64_t u1 = u2 >> 9;
    data = ( ( data & u1 ) << 9 ) | (data & ~u2);
    n_bytes -= 1;
  }

  uint64_t atoi(int length) const {
    assert(length >= 0 && length <= max_bytes);
    constexpr static uint64_t b_data = repeat<'0'>();
    uint64_t calculation_data = data - (b_data << (9 * (max_bytes - length)));
    auto shift = max_bytes - length;
    calculation_data = (calculation_data & neg_guard_mask) >> (9 * shift);
    constexpr static uint64_t odd_mask = ( UINT64_C(0xFF) << 1 ) | ( UINT64_C(0xFF) << 19 ) | ( UINT64_C(0xFF) << 37 ) | ( UINT64_C(0xFF) << 55 );

    const uint64_t mul2 = (calculation_data >> 8);
    const uint64_t mul8 = (calculation_data >> 6);
    
    calculation_data += mul2 + mul8;
    calculation_data &= odd_mask;
   
    const uint64_t mul64 = (calculation_data >> 12);
    const uint64_t mul32 = (calculation_data >> 13);
    const uint64_t mul4 = (calculation_data >> 16);

    calculation_data += mul4 + mul32 + mul64;

    uint64_t low = ( calculation_data >>  1 ) & UINT64_C(0x1FFFF);
    uint64_t hi =  ( calculation_data >> 33 ) & UINT64_C(0x1FFFF);
    low += hi << 6;
    hi += hi << 4;
    hi += hi << 5;
    return low + hi;
  }
  friend class mask_type;
};

#endif //sixit_core_cpual_simd_simd_byte_buffer64_h_included

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