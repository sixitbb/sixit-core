# sixit-core
sixit/core is a collection of small but all-important libs which lay foundation for all the other sixit/ libs. 

## WARNING: APIs are SUBJECT TO CHANGE WITHOUT NOTICE
We are currently at version 0.0.1, and we will NOT commit to APIs being stable at least until v0.1.0... 

## Supported platforms.
In our speak, "platform" = "CPU+Compiler+OS". 

### Supported CPUs:
- x64
- ARM64
- RISC-V64
- WASM32

Planned:
- s390x
- WASM64
- Power

NOT planned by ourselves, but we welcome pull requests as long as they're concentrated within sixit/cpual;
- legacy 64-bit CPUs such as SPARC, PA-RISC, even Itanium - as long as there is at least one recent major compiler supporting them, AND you have a working box to run CI on (!).

NOT planned by ourselves, but we welcome pull requests, as long as you have working box to run CI on (!); note, however, that they're unlikely to be 100% optimal (lots of sixit:: code is optimized for 64-bit arithmetics):
- x86
- ARM32
- RISC-V32
- anything-else which has a standard C++ compiler

NOT likely:
- legacy CPUs/MCUs which are not supported by more-or-less-recent stock C++ compilers (such as AVR8)

### Supported Compilers:
- Clang starting from 12.x (we hope to add support starting from 10.x too, less in unlikely)
- MSVC starting from _MSC_VER=19.32 (we hope to add support for 19.29 too, less in unlikely)
- GCC starting from 11.x (we hope to add support for 10.x too, less in unlikely)

NOT planned by ourselves, but we welcome pull requests as long as they're NOT spread all over the lib, AND you have working box to run CI on (!):
- ICX, NVCC, newer (Clang-based) XLC, any other Clang-frontend-based

Unlikely:
- legacy custom-frontend-based compilers, such as ICC, pre-clang-XLC, etc.

### Supported OSs:
- Linux
- Windows
- MacOS X
- Android
- iOS

Planned:
- FreeBSD
- WASM/Sandbox (our own pseudo-OS to run purely-computing things)
- WASM/WASI
- Consoles (XBox/PS/Switch)

NOT planned by ourselves, but we welcome pull requests as long as they're concentrated within sixit/osal (to be released soon, see below);
- pretty much anything which makes at least sense - AND for which you have working box to test it on (!).

### Currently Officially Tested Configurations
sixit/sarge CI Tier 1 (cannot merge to trunk unless CI passes):
- Linux/x64/GCC 11.3
- Linux/x64/Clang 12
- Win/x64/MSVC 19.32
- MacOS/M2/Apple-Clang
- Android/ARM (build only)
- Android/x64 (build only, adding simulator tests in progress)
- iOS/ARM (build only)

sixit/sarge CI Tier 3 (run manually on regular basis):
- Linux/RISC-V64/GCC 12

Planned:
- Win/ARM/MSVC
- dual builds ("legacy" and "new") for GCC/x64
- dual builds ("legacy" and "new") for Clang/x64
- dual builds ("legacy" and "new") for MSVC/x64

## Included files and libs
- sixit/core.h - very basic stuff, CPU-COMPILER-OS detection
- sixit/lwa.h - Legacy WorkArounds. The idea is to provide wrappers for new language features, so if the feature is available on the target compiler - we ALWAYS use it, but for older compilers - we're trying to simulate it to the best of our abilities.
- sixit/guidelines.h - helpers for our own guidelines , which we follow across our sixit libs. You're welcome to use them too ;-). 
- sixit/cpual - encapsulating CPU specifics (for non-standardized yet features such as 64x64->128bit multiplication, functions for deterministic-float inline-asm-based support, clock-count a-la RDTSC, and of course, SIMD).
- sixit/profiler - a library for cross-platform performance measurements, especially oriented towards production (!) - with planned ways to provide the data for Zabbix/Nagios. Has very cheap sensors (RDTSC-like) for all supported CPUs, except for WASM.
- sixit/logger.h - concept of logger (we do not force logger down your throat, you can use whatever you want - as long as it conforms to logger concept). We also provide basic loggers ourselves (see also on v0.0.2 below)
- sixit/compileral - compiler isolation layer. Currently very small (but still provides valuable stuff such as printable and supposedly-consistent-across-compilers printing of typeid).
- sixit/units - a mini-lib to support measurement units, and most importanly - [Dimension analysis], which we're using in places such as sixit/geometry and sixit/physics (see below).
- sixit/testing - a micro-framework for fully cross-platform testing aimed at CI regression testing, inspired by lest. 

## Building
sixit/core is a HEADER-ONLY LIB, no build is really necessary. 
NB: we MAY consider generating a non-header-only version, IF there is a SUBSTANTIAL demand for it. Note, however, that (a) template stuff (and we have LOTS of it) has to stay in headers, AND (b) moving this kind of small things to .cpps tends to hurt performance a bit (due to lack of inlining), which may or may not be eliminated by specifying LTO.
- we also will provide a separate sixit-core-test lib. 

## Plans for v0.0.2
- further code cleanup
- additional loggers: file logger, combining adapter, optional wrapper for spdlog. 

## Other sixit libs we already have and plan to release soon:
- sixit/dmath - cross-platform deterministic math lin
- sixit/geometry - geometrical primitives and algorithms (both 2D and 3D), with cross-platform deterministic support
- sixit/rw - serialization lib aiming top efficiency, and able to support ABSOLUTELY ANY format (JSON, protobuf, custom binary competing with FlatBuffers and Cap'nProto, ypu name it). Also supports arbitrary hashing (as a pseudo-serialization). 
- sixit/graphics - comprehensive support for meshes (incl. skinning), textures, etc. Materials are PBR-only though. Also will contain basic helpers such as decimator half-edge-oriented framework (based directly on [A General Framework for Mesh Decimation
](https://www.graphics.rwth-aachen.de/media/papers/mesh.pdf)).
- sixit/physics - simple physics, with cross-platform deterministic support
   + currently only rigid body physics , but we plan to add more
- sixit/osal - os abstraction library; abstracts less common features which are not a part of the std:: (yet?).
- sixit/wasm - abstracting wasm engines (the same host can use any of supported engines, from wasm3 to wasm2c, with adding more being easy)

### CI
- sixit/sarge: opinionated CI which is C++-aware, tightly integrated with build, has a pre-merge CI check option (prevents BS from ever getting into your trunk, HIGHLY recommended), supports build in identical VMs, and supports our "cpp-tight-ship" development model with lots of guideline checks (can be adjusted to your needs). Sarge is highly modular too.

### Bloom Game Engine
- sixit/bloom: a gaming engine, which uses most of the things above, and aiming to support modern development practices (WASM for scripting, PBR for materials, Vulkan & Metal for rendering), and aims to fully support Web too.
   + NB: some OPTIONAL features of Bloom engine represent our know-how, and will NOT be open-sourced. 
