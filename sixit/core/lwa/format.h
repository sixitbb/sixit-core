/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin
*/

/** \file format.h
\brief **sixit/core/lwa/format.h**: entry point for fmt library
 */

#ifndef sixit_core_lwa_format_h_included
#define sixit_core_lwa_format_h_included

#ifdef SIXIT_USE_EXTERNAL_FMT
#  include "fmt/format.h" // if external lest library is used, it is expected to be at the project root directory
#endif

#ifdef SIXIT_USE_EXTERNAL_FMT
#  include "fmt/chrono.h" // if external lest library is used, it is expected to be at the project root directory
#endif

#ifndef SIXIT_USE_EXTERNAL_FMT

#  ifdef FMT_FORMAT_H_
#    error format.h has already been included. If an alternative lest library is used, add SIXIT_USE_EXTERNAL_FMT compiler option, place fmt to the project root directory, and add its include directory to project include directories -I<...>
#  endif //  FMT_FORMAT_H_

#  ifdef SIXIT_COMPILER_MSVC
#    pragma warning (disable : 4459 )
#    pragma warning (disable : 4996 )
#  endif // SIXIT_COMPILER_MSVC
#  define FMT_HEADER_ONLY
#  include "3rdparty/fmt/include/fmt/printf.h"
#  include "3rdparty/fmt/include/fmt/chrono.h" // subject to remove
namespace sixit::lwa {
	namespace fmt = ::fmt;
}

#endif // SIXIT_USE_EXTERNAL_FMT

#endif // sixit_core_lwa_format_h_included


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
