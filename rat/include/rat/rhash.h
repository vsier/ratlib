// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RHASH_H_
#define RAT_RHASH_H_

#include <rat/common.h>


typedef uint32_t rhash_val;

// Incremental hash state. The current implementation assumes little-endian
// object representation for rhash_val words.
typedef struct
{
    rhash_val val[4];
    rhash_val rem;
    size_t totalSize;
} rhash;

// key0/key1 select the keyed hash stream.
void rhash_init(rhash *out, rhash_val key0, rhash_val key1);
// Adds buf[0..bufSize) to the stream.
void rhash_add(rhash *p, const void *buf, size_t bufSize);
// Finalizes and returns the current stream hash without modifying p.
rhash_val rhash_result(rhash *p);

// Computes a hash result in one call.
rhash_val rhash_calc(rhash_val key0, rhash_val key1, const void *buf, size_t bufSize);

#endif // !RAT_RHASH_H_
