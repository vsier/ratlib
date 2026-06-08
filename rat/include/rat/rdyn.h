// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RDYN_H_
#define RAT_RDYN_H_

#include <rat/ralc.h>
#include <rat/rarr.h>


// Dynamic array with element storage owned by the supplied allocator. Functions
// that grow or shrink may move buf and invalidate element pointers.
typedef struct
{
    ralc_iface *ralc;
    unsigned char *buf;
    size_t elemSize;
    size_t cap;
    size_t count;
    bool allowShrink; // False keeps capacity after removes; true permits shrink.
} rdyn;

// Initializes an empty array of elemSize-byte elements.
void rdyn_init(rdyn *p, size_t elemSize, ralc_iface *ralc_if);

// Inserts count elements at index. index must be no greater than p->count.
void *rdyn_insert(rdyn *p, size_t index, size_t count);
// Resizes the element run at index from count to newCount. The old run must be
// in bounds.
void *rdyn_resize(rdyn *p, size_t index, size_t count, size_t newCount);
// Removes count elements at index. The removed run must be in bounds.
void *rdyn_remove(rdyn *p, size_t index, size_t count);

// Sets capacity. newCap 0 clears the array.
bool rdyn_setCap(rdyn *p, size_t newCap);
// Frees the owned buffer through p->ralc and leaves p empty.
void rdyn_clear(rdyn *p);

// Inserts count elements at the end.
#define rdyn_push(_rdyn_p, _count)          rdyn_insert(_rdyn_p, (_rdyn_p)->count, _count)
// Removes count elements from the end. count must not exceed the current count.
#define rdyn_pop(_rdyn_p, _count)           rdyn_remove(_rdyn_p, (_rdyn_p)->count - (_count), _count)
// Returns a void pointer to element index. Bounds are the caller's responsibility.
#define rdyn_at(_rdyn_p, _index)            ((void *)((_rdyn_p)->buf + (_index) * (_rdyn_p)->elemSize))
// Returns a typed pointer to element index. Bounds are the caller's responsibility.
#define rdyn_at_t(_rdyn_p, _t, _index)      ((_t *)(_rdyn_p)->buf + (_index))
// Returns a void pointer to element count - 1 - index.
#define rdyn_last(_rdyn_p, _index)          ((void *)((_rdyn_p)->buf + ((_rdyn_p)->count - 1 - (_index)) * (_rdyn_p)->elemSize))
// Returns a typed pointer to element count - 1 - index.
#define rdyn_last_t(_rdyn_p, _t, _index)    ((_t *)(_rdyn_p)->buf + ((_rdyn_p)->count - 1 - (_index)))

// Searches the ascending sorted dynamic array. If target is present, out_index receives
// the leftmost matching index; otherwise it receives the insertion index.
#define rdyn_binSearch(_rdyn_p, _cmp_fn, _target_p, _out_index) rarr_binSearch((_rdyn_p)->buf, (_rdyn_p)->elemSize, (_rdyn_p)->count, _cmp_fn, _target_p, _out_index)
// Searches the descending sorted dynamic array. If target is present, out_index receives
// the rightmost matching index; otherwise it receives the insertion index.
#define rdyn_binSearch_desc(_rdyn_p, _cmp_fn, _target_p, _out_index) rarr_binSearch_desc((_rdyn_p)->buf, (_rdyn_p)->elemSize, (_rdyn_p)->count, _cmp_fn, _target_p, _out_index)

#endif
