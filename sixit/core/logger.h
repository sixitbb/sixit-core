/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko
*/
#ifndef sixit_core_logger_h_included
#define sixit_core_logger_h_included

#include "sixit/core/lwa.h"

namespace sixit::loggers
{
enum class log_level
{
    trace = 0,
    debug,
    info,
    warn,
    err,
    critical,
    off
};

#ifdef SIXIT_LWA_OPTIONAL_CONCEPT

/*
 logger should provide variadic template functions similar to sixit::lwa::fmt::format/fmt::print
*/
template <typename logger_t, typename... args_t>
concept logger_concept = requires(logger_t& logger, log_level level, const char* fmt_string, args_t... args) {
    { logger.flush() } -> std::same_as<void>;
    { logger.default_level() } -> std::same_as<log_level>;
    logger.log(level, fmt_string, args...);
    logger.log(fmt_string, args...);
    logger.trace(fmt_string, args...);
    logger.debug(fmt_string, args...);
    logger.info(fmt_string, args...);
    logger.warn(fmt_string, args...);
    logger.err(fmt_string, args...);
    logger.critical(fmt_string, args...);
};

#endif // SIXIT_LWA_OPTIONAL_CONCEPT

} // namespace sixit::loggers

#endif // sixit_core_logger_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko

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
