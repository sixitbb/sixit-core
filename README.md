# sixit-core
sixit/core is a collection of small but all-important libs which lay foundation for all the other sixit libs. 

## WARNING: APIs are SUBJECT TO CHANGE WITHOUT NOTICE
We are currently at version 0.0.1, and we will NOT commit to APIs being stable at least until v0.1.0... 

## Included files and libs
- sixit/core.h - very basic stuff, CPU-COMPILER-OS detection
- sixit/lwa.h - Legacy WorkArounds. The idea is to provide wrappers for new language features, so if the feature is available on the target compiler - we ALWAYS use it, but for older compilers - we're trying to simulate it to the best of our abilities.
- sixit/guidelines.h - helpers for our own guidelines , which we follow across our sixit libs. You're welcome to use them too ;-). 
- sixit/cpual - encapsulating CPU specifics (for non-standardized yet features such as 64x64->128bit multiplication, functions for deterministic-float inline-asm-based support, clock-count a-la RDTSC, and of course, SIMD).
- sixit/profiler - a library for cross-platform performance measurements, especially oriented towards production (!) - with planned ways to provide the data for Zabbix/Nagios. Has very cheap sensors (RDTSC-like) for all supported CPUs, except for WASM.
- sixit/logger.h - concept of logger (we do not force logger down your throat, you can use whatever you want - as long as it conforms to logger concept). We also provide basic loggers ourselves (see also on v0.0.2 below)
- sixit/compileral - compiler isolation layer. Currently very small (but still provides valuable stuff such as printable and supposedly-consistent-across-compilers printing of typeid).
- sixit/units - a mini-lib to support measurement units, and most importanly - [Dimension analysis](https://en.wikipedia.org/wiki/Dimensional_analysis), which we're using in places such as sixit/geometry and sixit/physics (see below).
- sixit/containers - some of our custom containers, which include such things (not necessarily released yet) as index-preserving-deque, map-with-best-O(1)-case-and-worst-O(log(N))-case, unordered-map-with-guaranteed-O(1) (especially useful in compile-time), and so on. 
- sixit/testing - a micro-framework for fully cross-platform testing aimed at CI regression testing, inspired by lest.

## Supported Platforms
We're currently supporting 4 CPU architectures, 3 compilers, and 5 operating systems (with more WIP/Planned). For details, please refer to [Supported platforms for sixit C++ Libs](https://github.com/sixitbb/.github/blob/main/profile/cpp-supported-platforms.md).

## Building
sixit/core is a HEADER-ONLY LIB, no build is really necessary. 
NB: we MAY consider generating a non-header-only version, IF there is a SUBSTANTIAL demand for it. Note, however, that (a) template stuff (and we have LOTS of it) has to stay in headers, AND (b) moving this kind of small things to .cpps tends to hurt performance a bit (due to lack of inlining), which may or may not be eliminated by specifying LTO.
- we also will provide a sixit/core/tests folder - that's what CMakeLists.txt will really build and run, or a separate sixit-core-test lib.

## Plans for v0.0.2
- further code cleanup
- adding SIXIT_CORE_VERSION (and minimum checks in dependent sixit libs)
- releasing sixit/core/containers, sixit/lwa/constexpr_vector.
- releasing sixit/core/profiler, ideally with file support (enabling simple Zabbix/Nagios monitoring)
- additional loggers: file logger, combining adapter, optional wrapper for spdlog.
- improving+expanding sixit/units: adding systems template parameter, adding systems, adding popular units for L and T, adding units maths.
- releasing rudimentary sixit/core/compileral
- MAYBE: adding support for WASM32
