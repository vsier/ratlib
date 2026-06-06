// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RALCGEN_H_
#define RAT_RALCGEN_H_

#include <rat/ralc.h>


// General-purpose allocator over a fixed caller-provided buffer. It can reuse
// freed blocks and can grow/shrink/reallocate the last block in place.
typedef struct
{
    unsigned char *buf;
    size_t cap;
    size_t size;
    ralc_iface ifaceRalc;
} ralcGen;

// Initializes p to allocate from buf[0..bufSize).
void ralcGen_init(ralcGen *p, void *buf, size_t bufSize);

// Allocates size bytes. Returned pointers stay valid until freed, reallocated,
// or until clear.
void *ralcGen_alloc(ralcGen *p, size_t size, size_t *out_actualSize);
// Resizes ptr to newSize bytes. A NULL ptr requests allocation.
void *ralcGen_realloc(ralcGen *p, void *ptr, size_t newSize, size_t *out_actualNewSize);

// Frees a non-NULL live object previously returned by p.
void ralcGen_free(ralcGen *p, void *ptr);

// Invalidates all outstanding objects and makes the whole buffer reusable.
void ralcGen_clear(ralcGen *p);

// Fills out_usage with current block, hole, and allocator metadata accounting.
void ralcGen_getUsage(ralcGen *p, ralc_usage *out_usage);

#endif
