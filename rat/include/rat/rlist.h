// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RLIST_H_
#define RAT_RLIST_H_

#include <rat/ralc.h>


// Doubly linked list whose nodes are allocated through parentRalc. The returned
// object pointers refer to node payloads, not node headers.
typedef struct
{
    ralc_iface *parentRalc;
    void *first;
    void *last;
    size_t count;
    ralc_iface ifaceRalc;
} rlist;

// Initializes an empty list.
void rlist_init(rlist *p, ralc_iface *parentRalc_p);

// Returns the backing allocator request size for one list element payload.
size_t rlist_elemAllocSize(size_t objSize);

// Inserts before obj_p, or at the back when obj_p is NULL.
void *rlist_insert(rlist *p, const void *obj_p, size_t size, size_t *out_actualSize);
// Resizing can move the resized element. The return value is the current
// pointer to that element.
void *rlist_resize(rlist *p, void *obj_p, size_t newSize, size_t *out_actualNewSize);
// Removes count elements starting at obj_p and returns the following element,
// or NULL when removal reached the end. obj_p and count must describe a live
// in-bounds run.
void *rlist_remove(rlist *p, void *obj_p, size_t count);

void rlist_clear(rlist *p);

// Returns the previous element, or NULL when obj_p is first.
void *rlist_prev(const void *obj_p);
// Returns the next element, or NULL when obj_p is last.
void *rlist_next(const void *obj_p);

// Returns obj_p's zero-based index. obj_p must be a live element in p.
size_t rlist_index(rlist *p, const void *obj_p);
// Returns the element at index. index must be less than p->count.
void *rlist_at(rlist *p, size_t index);

void rlist_getUsage(rlist *p, ralc_usage *out_usage);

#define rlist_push(_list_p, _size, _out_actualSize)      rlist_insert(_list_p, NULL, _size, _out_actualSize)
#define rlist_pushFront(_list_p, _size, _out_actualSize) rlist_insert(_list_p, (_list_p)->first, _size, _out_actualSize)

#endif
