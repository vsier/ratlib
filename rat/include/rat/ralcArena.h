// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RALCARENA_H_
#define RAT_RALCARENA_H_

#include <rat/ralc.h>


// Chunked bump allocator. Individual objects remain allocated until clear or
// mark/rewind discards groups of allocations.
typedef struct
{
    ralc_iface *parentRalc;
    void *chunks;
    size_t chunkSize; // Default payload capacity for new chunks.
    size_t size;      // Total aligned user bytes currently live in the arena.
    ralc_iface ifaceRalc;
} ralcArena;

typedef size_t ralcArena_mark_t;

// Initializes p with parentRalc_p as the backing allocator. initChunkSize is the
// default payload capacity for new chunks.
void ralcArena_init(ralcArena *p, size_t initChunkSize, ralc_iface *parentRalc_p);

// Pushes size bytes onto the arena. A zero-size push reserves one byte before
// alignment, so every successful push returns a distinct pointer. out_actualSize
// receives the aligned size.
void *ralcArena_push(ralcArena *p, size_t size, size_t *out_actualSize);
// Returns the current arena byte position.
ralcArena_mark_t ralcArena_mark(ralcArena *p);
// Rewinds to a mark produced by this arena. The mark must not have been
// invalidated by clear or by rewinding below it.
void ralcArena_rewind(ralcArena *p, ralcArena_mark_t mark);

// Returns the parent allocator request size for one arena chunk with chunkSize
// bytes of payload capacity.
size_t ralcArena_chunkAllocSize(size_t chunkSize);

// Frees all chunks owned by the arena through the parent allocator.
void ralcArena_clear(ralcArena *p);

// Fills out_usage with parent allocator accounting adjusted so chunk headers
// are system storage and unused chunk tails are holes.
void ralcArena_getUsage(ralcArena *p, ralc_usage *out_usage);

#endif
