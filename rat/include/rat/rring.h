// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RRING_H_
#define RAT_RRING_H_

#include <rat/common.h>


// Non-owning fixed-capacity ring buffer over caller-provided storage.
typedef struct
{
    unsigned char *buf;
    size_t elemSize;
    size_t cap;
    size_t first;
    size_t count;
} rring;

// Initializes an empty ring over buf. buf must have elemSize * cap bytes.
void rring_init(rring *p, void *buf, size_t elemSize, size_t cap);

// Adds one uninitialized element at the back, or returns NULL when full.
void *rring_push(rring *p);
// Adds one uninitialized element at the front, or returns NULL when full.
void *rring_pushFront(rring *p);
// Removes count elements from the back. count must not exceed p->count.
void rring_pop(rring *p, size_t count);
// Removes count elements from the front. count must not exceed p->count.
void rring_popFront(rring *p, size_t count);

// Returns element index from the front. Bounds are the caller's responsibility.
void *rring_at(const rring *p, size_t index);
// Returns element index from the back. Bounds are the caller's responsibility.
void *rring_last(const rring *p, size_t index);

// Removes all elements without modifying stored bytes.
void rring_clear(rring *p);

#endif
