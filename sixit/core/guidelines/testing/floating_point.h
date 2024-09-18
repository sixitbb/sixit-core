/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors:
*/

#ifndef sixit_core_guidelines_testing_floating_point_h_included
#define sixit_core_guidelines_testing_floating_point_h_included

#include "sixit/core/lwa.h"
#include <cassert>
#include <tuple>

namespace sixit::test
{
    float _make_float(bool sign, int32_t exp, int32_t frac)
    {
        assert(exp <= 0xff && frac <= 0x7fffff);
        int32_t bit_value = 0;
        bit_value = bit_value | (frac & 0x007FFFFF);
        bit_value = bit_value | ((exp << 23) & 0x7f800000);
        bit_value = bit_value | (sign << 31);

        return lwa::bit_cast<float>(bit_value);
    }

    std::tuple<bool, int32_t, int32_t> _split_float(float value)
    {
        int32_t bit_value = lwa::bit_cast<int32_t>(value);
        bool sign = bit_value & 0x80000000;
        int32_t exp = (bit_value & 0x7f800000) >> 23;
        int32_t frac = bit_value & 0x007FFFFF;

        return std::make_tuple(sign, exp, frac);
    }

    double _make_double(bool sign, int32_t exp, int64_t frac)
    {
        assert(exp <= 0x7ff && frac <= 0xfffffffffffff);
        int64_t bit_value = 0;
        bit_value = bit_value | (frac & 0xfffffffffffff);
        bit_value = bit_value | ((static_cast<int64_t>(exp) << 52) & 0x7ff0000000000000);
        bit_value = bit_value | (static_cast<int64_t>(sign) << 63);

        return lwa::bit_cast<double>(bit_value);
    }

    std::tuple<bool, int32_t, int64_t> _split_double(double value)
    {
        int64_t bit_value = lwa::bit_cast<int64_t>(value);
        bool sign = bit_value & 0x8000000000000000;
        int32_t exp = (bit_value & 0x7ff0000000000000) >> 52;
        int64_t frac = bit_value & 0xfffffffffffff;

        return std::make_tuple(sign, exp, frac);
    }
    namespace
    {
        template<typename Func>
        void exhaustive_for_f(float f1, float f2, Func lambda)
        {
            const auto f1_split = _split_float(f1);
            const int32_t f1_exp = std::get<1>(f1_split);
            const int32_t f1_frac = std::get<2>(f1_split);

            const auto f2_split = _split_float(f2);
            const int32_t f2_exp = std::get<1>(f2_split);
            const int32_t f2_frac = std::get<2>(f2_split);

            assert(std::get<0>(f1_split) == std::get<0>(f2_split));

            const bool sign = std::get<0>(f1_split);

            const int32_t max_frac_for_float = 0x7fffff;

            if ((f1 <= f2 && !sign) || (f1 > f2 && sign))
            {
                for (int32_t exp = f1_exp; exp <= f2_exp; ++exp)
                {
                    int32_t max_frac = exp == f2_exp ? f2_frac : max_frac_for_float;

                    for (int32_t frac = exp == f1_exp ? f1_frac : 0; frac <= max_frac; ++frac)
                    {
                        lambda(_make_float(sign, exp, frac));
                    }
                }
            }
            else
            {
                for (int32_t exp = f1_exp; exp >= f2_exp; --exp)
                {
                    int32_t min_frac = exp == f2_exp ? f2_frac : 0;

                    for (int32_t frac = exp == f1_exp ? f1_frac : max_frac_for_float; frac >= min_frac; --frac)
                    {
                        lambda(_make_float(sign, exp, frac));
                    }
                }
            }
        }

        template<typename Func>
        void exhaustive_for_d(double d1, double d2, Func lambda)
        {
            const auto d1_split = _split_double(d1);
            const int32_t d1_exp = std::get<1>(d1_split);
            const int64_t d1_frac = std::get<2>(d1_split);

            const auto d2_split = _split_double(d2);
            const int32_t d2_exp = std::get<1>(d2_split);
            const int64_t d2_frac = std::get<2>(d2_split);

            assert(std::get<0>(d1_split) == std::get<0>(d2_split));

            const bool sign = std::get<0>(d1_split);

            const int64_t max_frac_for_float = 0xfffffffffffff;

            if ((d1 <= d2 && !sign) || (d1 > d2 && sign))
            {
                for (int32_t exp = d1_exp; exp <= d2_exp; ++exp)
                {
                    int64_t max_frac = exp == d2_exp ? d2_frac : max_frac_for_float;

                    for (int64_t frac = exp == d1_exp ? d1_frac : 0; frac <= max_frac; ++frac)
                    {
                        lambda(_make_double(sign, exp, frac));
                    }
                }
            }
            else
            {
                for (int32_t exp = d1_exp; exp >= d2_exp; --exp)
                {
                    int64_t min_frac = exp == d2_exp ? d2_frac : 0;

                    for (int64_t frac = exp == d1_exp ? d1_frac : max_frac_for_float; frac >= min_frac; --frac)
                    {
                        lambda(_make_double(sign, exp, frac));
                    }
                }
            }
        }
    }

    template<typename Func>
    void _exhaustive_for_exp(int32_t exp, int32_t initial_fraction, float f1, float f2, Func lambda)
    {
        assert(f1 <= f2);
        int32_t f1_frac = std::get<2>(_split_float(f1));
        int32_t f2_frac = std::get<2>(_split_float(f2));
        assert(f1_frac <= f2_frac);

        int32_t frac = initial_fraction >= f1_frac ? initial_fraction : f1_frac;

        for (;; ++frac) {
            assert(frac >= f1_frac);

            if (frac > f2_frac)
                return;

            lambda(_make_float(false, exp, frac));
        }
    }

    template<typename Func>
    void _exhaustive_for_nonneg_f(float f1, float f2, Func lambda)
    {
        assert(f1 >= 0.f && f2 >= 0.f);

        exhaustive_for_f(f1, f2, lambda);
    }

    template<typename Func>
    void _exhaustive_for_f(float f1, float f2, Func lambda)
    {
        if (f1 < 0.f && f2 > 0.f)
        {
            exhaustive_for_f(f1, lwa::bit_cast<float>(0X80800000), lambda);
            exhaustive_for_f(lwa::bit_cast<float>(0x800000), f2, lambda);
        }
        else if (f1 > 0.f && f2 < 0.f)
        {
            exhaustive_for_f(f1, lwa::bit_cast<float>(0x800000), lambda);
            exhaustive_for_f(lwa::bit_cast<float>(0X80800000), f2, lambda);
        }
        else
        {
            exhaustive_for_f(f1, f2, lambda);
        }
    }

    template<typename Func>
    void _exhaustive_for_exp_d(int32_t exp, int64_t initial_fraction, double d1, double d2, Func lambda)
    {
        assert(d1 <= d2);
        int64_t d1_frac = std::get<2>(_split_double(d1));
        int64_t d2_frac = std::get<2>(_split_double(d2));
        assert(d1_frac <= d2_frac);

        int64_t frac = initial_fraction >= d1_frac ? initial_fraction : d1_frac;

        for (;; ++frac) {
            assert(frac >= d1_frac);

            if (frac > d2_frac)
                return;

            lambda(_make_double(false, exp, frac));
        }
    }

    template<typename Func>
    void _exhaustive_for_nonneg_d(double d1, double d2, Func lambda)
    {
        assert(d1 >= 0. && d2 >= 0.);

        exhaustive_for_d(d1, d2, lambda);
    }

    template<typename Func>
    void _exhaustive_for_d(double d1, double d2, Func lambda)
    {
        if (d1 < 0. && d2 > 0.)
        {
            exhaustive_for_d(d1, lwa::bit_cast<double>(0X8010000000000000), lambda);
            exhaustive_for_d(lwa::bit_cast<double>(0x10000000000000), d2, lambda);
        }
        else if (d1 > 0. && d2 < 0.)
        {
            exhaustive_for_d(d1, lwa::bit_cast<double>(0x10000000000000), lambda);
            exhaustive_for_d(lwa::bit_cast<double>(0X8010000000000000), d2, lambda);
        }
        else
        {
            exhaustive_for_d(d1, d2, lambda);
        }
    }
}

#endif //sixit_core_guidelines_testing_floating_point_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors:

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