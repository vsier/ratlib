// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RARR_H_
#define RAT_RARR_H_

#include <rat/common.h>
#include <rat/rcmp.h>


// Raw array shifting helpers. The caller owns storage capacity and bounds.
// Passing out-of-range index/count values or overflowing size calculations is
// invalid. Zero-byte memmove with NULL pointers is assumed to be valid.

// Inserts count elements at index. arrLen is the array length after insertion.
void *rarr_insert(void *arr, size_t elemSize, size_t arrLen, size_t index, size_t count);

// Removes count elements at index. arrLen is the array length before removal.
void *rarr_remove(void *arr, size_t elemSize, size_t arrLen, size_t index, size_t count);

// Resizes the element run at index from oldCount to newCount.
void *rarr_resize(void *arr, size_t elemSize, size_t arrLen, size_t index, size_t oldCount, size_t newCount);

// Searches an ascending sorted array. If target is present, out_index receives
// the leftmost matching index; otherwise it receives the insertion index.
bool rarr_binSearch(const void *arr, size_t elemSize, size_t arrLen, rcmp cmp_fn, const void *target_p, size_t *out_index);
// Searches a descending sorted array. If target is present, out_index receives
// the rightmost matching index; otherwise it receives the insertion index.
bool rarr_binSearch_desc(const void *arr, size_t elemSize, size_t arrLen, rcmp cmp_fn, const void *target_p, size_t *out_index);

#endif
