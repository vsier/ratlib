<!--
SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
SPDX-License-Identifier: MIT
-->

# ratlib

ratlib is a small C99 utility library for allocators, collections, and low-level
helpers.

[![linux-gcc](https://github.com/vsier/ratlib/actions/workflows/ci-linux-gcc.yml/badge.svg)](https://github.com/vsier/ratlib/actions/workflows/ci-linux-gcc.yml)
[![linux-clang](https://github.com/vsier/ratlib/actions/workflows/ci-linux-clang.yml/badge.svg)](https://github.com/vsier/ratlib/actions/workflows/ci-linux-clang.yml)
[![macos-clang](https://github.com/vsier/ratlib/actions/workflows/ci-macos-clang.yml/badge.svg)](https://github.com/vsier/ratlib/actions/workflows/ci-macos-clang.yml)
[![windows](https://github.com/vsier/ratlib/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/vsier/ratlib/actions/workflows/ci-windows.yml)

## Functionality

- [`common.h`](rat/include/rat/common.h): common standard includes, generated
  platform configuration, export macros, and alignment spelling.
- [`extra.h`](rat/include/rat/extra.h): small helper macros for member
  introspection, container-of conversion, temporary compound literals,
  alignment, and array length.
- [`ralc.h`](rat/include/rat/ralc.h): generic allocator interface and usage
  accounting shared by allocator implementations.
- [`ralcGen.h`](rat/include/rat/ralcGen.h): general-purpose allocator over a
  caller-provided fixed buffer, with free/reuse and realloc support.
- [`ralcArena.h`](rat/include/rat/ralcArena.h): chunked bump allocator with
  clear and mark/rewind for discarding groups of allocations.
- [`ralcPool.h`](rat/include/rat/ralcPool.h): fixed-size pool allocator with
  stable slots and chunk growth through a parent allocator.
- [`ralcCustom.h`](rat/include/rat/ralcCustom.h): adapter for user-provided
  allocator callbacks.
- [`rarr.h`](rat/include/rat/rarr.h): non-owning raw array helpers for
  insert/remove/resize and ascending or descending binary search.
- [`rdyn.h`](rat/include/rat/rdyn.h): allocator-backed dynamic array with
  push/pop, resizing, optional shrinking, indexed access, and binary search
  wrappers.
- [`rlist.h`](rat/include/rat/rlist.h): allocator-backed doubly linked list
  whose public element pointers refer directly to payload storage.
- [`rmap.h`](rat/include/rat/rmap.h): hash map with copied variable-size keys
  and values, customizable hash/equality callbacks, and whole-map iteration.
- [`rring.h`](rat/include/rat/rring.h): non-owning fixed-capacity ring buffer
  over caller-provided storage.
- [`rhash.h`](rat/include/rat/rhash.h): incremental keyed 32-bit hash.
- [`rcrc.h`](rat/include/rat/rcrc.h): bit-by-bit CRC calculation for custom
  parameters and predefined CRC algorithms.
- [`rcmp.h`](rat/include/rat/rcmp.h): comparator functions for buffers,
  strings, numeric objects, floating-point objects, and pointer variants.
- [`rbit.h`](rat/include/rat/rbit.h): bit, flag, mask, and field macros for
  unsigned integer values.

## Build

Requirements:

- CMake 3.23 or newer.
- A C99 compiler.

Configure and build from the repository root:

```sh
cmake -S . -B build
cmake --build build --config Release
```

Build and run the tests:

```sh
cmake -S . -B build -DRAT_BUILD_TESTS=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

Build the benchmark executable:

```sh
cmake -S . -B build -DRAT_BUILD_BENCHMARKS=ON
cmake --build build --config Release --target rat_benchmarks
build/benchmarks/Release/rat_benchmarks 4096 100
```

Install the library:

```sh
cmake --install build --config Release --prefix install
```

To use the library from another CMake project, add the ratlib repository root
and link against the `rat` target:

```cmake
add_subdirectory("ratlib")
target_link_libraries(my_target PRIVATE rat)
```

## License

ratlib is released under the MIT License. Copyright (c) 2026
[vsier](https://github.com/vsier). See [`LICENSE`](LICENSE).
