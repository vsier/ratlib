// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RLIST_H_
#define RAT_RLIST_H_

#include <rat/ralc.h>


// Doubly linked list whose nodes are allocated through parentRalc. Element
// pointers address payload storage; NULL marks both ends of the list.
typedef struct
{
    ralc_iface *parentRalc;
    void *first;  // First live element, or NULL when empty.
    void *last;   // Last live element, or NULL when empty.
    size_t count; // Live element count.
    ralc_iface ifaceRalc;
} rlist;

// Initializes an empty list.
void rlist_init(rlist *p, ralc_iface *parentRalc_p);

// Inserts before obj_p, or at the back when obj_p is NULL.
void *rlist_insert(rlist *p, const void *obj_p, size_t size);
// Resizing can move the resized element. The return value is the current
// pointer to that element.
void *rlist_resize(rlist *p, void *obj_p, size_t newSize);
// Removes count elements starting at obj_p and returns the following element,
// or NULL when removal reached the end. obj_p and count must describe a live
// in-bounds run.
void *rlist_remove(rlist *p, void *obj_p, size_t count);

// Returns the previous element, or NULL when obj_p is first.
void *rlist_prev(const void *obj_p);
// Returns the next element, or NULL when obj_p is last.
void *rlist_next(const void *obj_p);
// Returns the backing allocator request size for one list element payload.
size_t rlist_elemAllocSize(size_t objSize);
// Returns the actual payload size of a live element.
size_t rlist_elemSize(const void *obj_p);

// Returns obj_p's zero-based index. obj_p must be a live element in p.
size_t rlist_index(rlist *p, const void *obj_p);
// Returns the element at index. index must be less than p->count.
void *rlist_at(rlist *p, size_t index);

// Fills out_usage with parent allocator accounting adjusted.
void rlist_getUsage(rlist *p, ralc_usage *out_usage);

// Frees every node and leaves p empty.
#define rlist_clear(_list_p)           (rlist_remove(_list_p, (_list_p)->first, (_list_p)->count))
// Appends one element and returns its payload pointer.
#define rlist_push(_list_p, _size)      rlist_insert(_list_p, NULL, _size)
// Prepends one element and returns its payload pointer.
#define rlist_pushFront(_list_p, _size) rlist_insert(_list_p, (_list_p)->first, _size)

#endif
