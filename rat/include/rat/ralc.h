// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RALC_H_
#define RAT_RALC_H_

#include <rat/common.h>


// Allocator usage accounting returned by getUsage functions.
typedef struct
{
    size_t blockCount; // Live user-visible allocation count.
    size_t blockSize;  // Total bytes in live user-visible allocations.
    size_t holeCount;  // Reusable or uncommitted payload region count.
    size_t holeSize;   // Total reusable or uncommitted payload bytes.
    size_t systemSize; // Metadata and other bytes not available as payload.
} ralc_usage;

typedef unsigned char ralc_kind;
#define RALC_GENERAL ((ralc_kind)1)
#define RALC_LIST    ((ralc_kind)2)
#define RALC_ARENA   ((ralc_kind)3)
#define RALC_POOL    ((ralc_kind)4)
#define RALC_CUSTOM  ((ralc_kind)5)

// Type-erased allocator interface dispatched by ralc functions.
typedef struct
{
    ralc_kind kind;
} ralc_iface;

// Dispatches allocation through p->kind. out_actualSize receives the allocator's
// rounded user size.
void *ralc_alloc(ralc_iface *p, size_t size, size_t *out_actualSize);

// A NULL ptr requests allocation. Non-NULL realloc is allocator-specific:
// unsupported implementations return NULL without freeing the old object.
void *ralc_realloc(ralc_iface *p, void *ptr, size_t newSize, size_t *out_actualNewSize);

// Generic free follows the target allocator's semantics. For non-arena
// allocators, ptr must be a non-NULL live object from that allocator.
void ralc_free(ralc_iface *p, void *ptr);

// Fills out_usage with allocator-specific accounting.
void ralc_getUsage(ralc_iface *p, ralc_usage *out_usage);

#endif
