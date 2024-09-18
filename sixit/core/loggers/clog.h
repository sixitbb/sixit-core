/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin
*/
#ifndef sixit_core_loggers_clog_h_included
#define sixit_core_loggers_clog_h_included

#include <iostream>
#include "sixit/core/logger.h"

namespace sixit::loggers
{

class clog
{
  public:
    clog() : m_stream(std::cout) {}
    explicit clog(std::ostream stream, log_level level) : m_stream(stream), m_level(level) {}
    explicit clog(std::ostream stream) : m_stream(stream) {}
    explicit clog(log_level level) : m_stream(std::cout), m_level(level) {}

    clog(clog&) = delete;
    clog& operator=(clog&) = delete;
    clog(clog&&) = default;
    clog& operator=(clog&&) = default;

    ~clog() {}

    constexpr log_level default_level() const
    {
        return log_level::trace;
    }

    void log(log_level lvl, std::string&& msg)
    {
        if (should_log(lvl))
        {
            auto formatted_msg = format_by_level(lvl, std::move(msg));
	    m_stream << formatted_msg;
            m_stream.flush();
        }
    }

    template <typename format_string_t, typename... args_t>
    void log(log_level lvl, format_string_t&& fstr, args_t&&... args)
    {
        log(lvl, sixit::lwa::fmt::format(std::forward<format_string_t>(fstr), std::forward<args_t>(args)...));
    }

    template <typename format_string_t, typename... args_t>
    void log(format_string_t&& fstr, args_t&&... args)
    {
        log(default_level(), sixit::lwa::fmt::format(std::forward<format_string_t>(fstr), std::forward<args_t>(args)...));
    }

    template <typename format_string_t, typename... args_t>
    void trace(format_string_t&& fstr, args_t&&... args)
    {
        log(log_level::trace, std::forward<format_string_t>(fstr), std::forward<args_t>(args)...);
    }

    template <typename format_string_t, typename... args_t>
    void debug(format_string_t&& fstr, args_t&&... args)
    {
        log(log_level::debug, std::forward<format_string_t>(fstr), std::forward<args_t>(args)...);
    }

    template <typename format_string_t, typename... args_t>
    void info(format_string_t&& fstr, args_t&&... args)
    {
        log(log_level::info, std::forward<format_string_t>(fstr), std::forward<args_t>(args)...);
    }

    template <typename format_string_t, typename... args_t>
    void warn(format_string_t&& fstr, args_t&&... args)
    {
        log(log_level::warn, std::forward<format_string_t>(fstr), std::forward<args_t>(args)...);
    }

    template <typename format_string_t, typename... args_t>
    void err(format_string_t&& fstr, args_t&&... args)
    {
        log(log_level::err, std::forward<format_string_t>(fstr), std::forward<args_t>(args)...);
    }

    template <typename format_string_t, typename... args_t>
    void critical(format_string_t&& fstr, args_t&&... args)
    {
        log(log_level::critical, std::forward<format_string_t>(fstr), std::forward<args_t>(args)...);
    }

    void flush()
    {
        m_stream.flush();
    }

  private:
    bool should_log(log_level level) const
    {
        return sixit::lwa::to_underlying(m_level) <=
            sixit::lwa::to_underlying(level);
    }

    std::string format_by_level(log_level lvl, std::string&& msg)
    {
        switch (lvl)
        {
        case log_level::trace:
            return sixit::lwa::fmt::format("{}\n", std::move(msg));
        case log_level::debug:
            return sixit::lwa::fmt::format("[debug] {}\n", std::move(msg));
        case log_level::info:
            return sixit::lwa::fmt::format("[info] {}\n", std::move(msg));
        case log_level::warn:
            return sixit::lwa::fmt::format("warning: {}\n", std::move(msg));
        case log_level::err:
            return sixit::lwa::fmt::format("error: {}\n", std::move(msg));
        case log_level::critical:
            return sixit::lwa::fmt::format("critical: {}\n", std::move(msg));
        default:
            return std::move(msg);
        }
    }

  private:
    std::ostream& m_stream;
    log_level m_level{ default_level() };
};

} // namespace sixit::loggers

#endif // sixit_core_loggers_clog_h_included

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
