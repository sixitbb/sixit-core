/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin
*/

/** \file test.h
\brief **sixit/core/lwa/format.h**: entry point for test library
 */

#ifndef sixit_core_testing_test_h_included
#define sixit_core_testing_test_h_included

#include <exception>
#include "sixit/core/lwa/format.h"

namespace sixit::test {

    class assumption_failed : public std::exception
    {
        std::string reason;
    public:
        assumption_failed() : reason("assumption failed") {}
        assumption_failed(const char* expl) : reason(expl) {}
        assumption_failed(std::string expl) : reason(expl) {}
        virtual const char* what() const noexcept {
            return reason.c_str();
        }
    };

    inline
    void presume(bool cond)
    {
        if (!cond)
            throw assumption_failed{};
    }

    inline
    void presume(bool cond, const char* reason)
    {
        if (!cond)
            throw assumption_failed{reason};
    }

    inline
    void presume(bool cond, const char* text, const char* file, uint32_t line)
    {
        if (!cond)
        {
            std::string reason = fmt::format("file \"{}\", line {}: assumption failed. Condition: {}", file, line, text);
            throw assumption_failed{ reason };
        }
    }


    template<class L>
    void presume_no_throw(L&& l, std::string reason)
    {
        try {
            l();
        }
        catch (...)
        {
            throw assumption_failed{ reason };
        }
    }

    template<class L>
    void presume_no_throw(L&& l)
    {
        presume_no_throw(std::move(l), "\'no-throw\' assumption failed");
    }

    template<class L>
    void presume_no_throw(L&& l, const char* file, uint32_t line)
    {
        std::string reason = fmt::format("file \"{}\", line {}: \'no-throw\' assumption failed", file, line);
        presume_no_throw(std::move(l), reason);
    }


    template<class L>
    void presume_throws(L&& l, std::string reason)
    {
        bool ok = false;
        try {
            l();
        }
        catch (...)
        {
            ok = true;
        }
        if (!ok)
        {
            throw assumption_failed{ reason };
        }
    }

    template<class L>
    void presume_throws(L&& l)
    {
        presume_no_throw(std::move(l), "\'throws\' assumption failed");
    }

    template<class L>
    void presume_throws(L&& l, const char* file, uint32_t line)
    {
        std::string reason = fmt::format("file \"{}\", line {}: \'throws\' assumption failed", file, line);
        presume_throws(std::move(l), reason);
    }


    template<class T>
    struct test_counters_holder
    {
        static int& all_tests_ctr()
        {
            static thread_local int _all_tests_ctr = 0;
            return _all_tests_ctr;
        }
        static int& failed_tests_ctr()
        {
            static thread_local int _failed_tests_ctr = 0;
            return _failed_tests_ctr;
        }
    };

    using test_counters = test_counters_holder<void>;

    template<class L, class ... Args>
    void test(std::string name, L&& l, Args&& ... args)
    {
        test_counters::all_tests_ctr() += 1;
        try {
            l(args...);
        }
        catch (const assumption_failed& e)
        {
            test_counters::failed_tests_ctr() += 1;
            fmt::print("test \"{}\": {}\n", name, e.what());
        }
        catch (const std::exception& e)
        {
            test_counters::failed_tests_ctr() += 1;
            fmt::print("test \"{}\": exception caught, what = {}\n", name, e.what());
        }
        catch (...)
        {
            test_counters::failed_tests_ctr() += 1;
            fmt::print("test \"{}\": unknown exception caught\n", name);
        }
    }

    inline
    bool print_test_results()
    {
        fmt::print("{} tests, {} failed\n", test_counters::all_tests_ctr(), test_counters::failed_tests_ctr());
        bool ret = !test_counters::failed_tests_ctr();
        test_counters::all_tests_ctr() = 0;
        test_counters::failed_tests_ctr() = 0;
        return ret;
    }

} // namespace sixit::test

#define PRESUME(expression) sixit::test::presume((!!(expression)), #expression, __FILE__, __LINE__)
#define PRESUME_THROWS(expression) sixit::test::presume_throws(expression, __FILE__, __LINE__)
#define PRESUME_NO_THROW(expression) sixit::test::presume_no_throw(expression, __FILE__, __LINE__)

#endif // sixit_core_testing_test_h_included


/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin

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
