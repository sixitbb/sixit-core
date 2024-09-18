/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherri Ignatchenko, Serhii Iliukhin
*/

#ifndef sixit_core_cpual_simd_simd_byte_buffer8_h_included
#define sixit_core_cpual_simd_simd_byte_buffer8_h_included

#include <stdint.h>

class simd_buffer8 {
private:
  uint8_t data = 0;
  int n_bytes = 0;
public:
  constexpr static int max_bytes = 1;
  using plain_type = uint8_t;
  
  int n_left() const { return n_bytes; }

  const uint8_t& get() const {
    return data;
  }


  inline void fill_from(const uint8_t& u) {
    data = u;
    n_bytes = 1;
  }

  inline void store(uint8_t* p) {
    *p = data;
  }

  inline void consume() {
    n_bytes = 0;
  }

  template <uint8_t b>
  inline bool equal_to() const {
    return data == b;    
  }

  template <uint8_t b>
  inline bool lt_than() const {
    return data < b;
  }

  template <uint8_t b>
  inline bool sub() {
    data -= b;
    return data < (255 - b);
  }

  inline void erase_and_shift_left() {
    n_bytes = 0;
  }

  inline uint64_t atoi() const {
    return n_bytes ? data - '0' : 0;
  }
};

#endif //sixit_core_cpual_simd_simd_byte_buffer8_h_included

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