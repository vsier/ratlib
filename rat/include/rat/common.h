// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_COMMON_H_
#define RAT_COMMON_H_

#include <rat/rat_config.h>
#include <rat/rat_export.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>


// Portable spelling for declarations that need explicit alignment.
#if defined(RAT_CONFIG_COMPILER_MSVC)
    #define RAT_ALIGNAS(_s) __declspec(align(_s))
#elif defined(RAT_CONFIG_COMPILER_GNU) || defined(RAT_CONFIG_COMPILER_Clang)
    #define RAT_ALIGNAS(_s) __attribute__((aligned(_s)))
#else
    #define RAT_ALIGNAS(_s)
#endif

#define RAT_CACHELINE RAT_ALIGNAS(RAT_CONFIG_CACHELINE_SIZE)

#endif
