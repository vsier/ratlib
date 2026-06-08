// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RALCPOOL_H_
#define RAT_RALCPOOL_H_

#include <rat/ralc.h>


// Fixed-size pool allocator. Slots are stable until freed or until clear.
typedef struct
{
    ralc_iface *parentRalc;
    void *chunks;
    void *freeSlots;
    size_t elemSize; // Fixed aligned payload size of each slot.
    size_t chunkCap; // Requested slot count for newly allocated chunks.
    size_t count;    // Number of currently allocated slots.
    ralc_iface ifaceRalc;
} ralcPool;

// Initializes p. elemSize is rounded up to alignment and minimum slot size;
// initChunkCap is normalized to one when zero.
void ralcPool_init(ralcPool *p, size_t elemSize, size_t initChunkCap, ralc_iface *parentRalc_p);

// Allocates one slot. A freed slot's contents are overwritten by the free list.
void *ralcPool_alloc(ralcPool *p);

// Frees a non-NULL live slot previously returned by p.
void ralcPool_free(ralcPool *p, void *ptr);

// Frees all chunks through the parent allocator and invalidates all slots.
void ralcPool_clear(ralcPool *p);

// Fills out_usage with parent allocator accounting adjusted so pool chunks are
// system storage, live slots are blocks, and free slots are holes.
void ralcPool_getUsage(ralcPool *p, ralc_usage *out_usage);

#endif
