// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RLIST_H_
#define RAT_RLIST_H_

#include <rat/ralc.h>


// Doubly linked list whose nodes are allocated through parentRalc. Element
// pointers address payload storage; p->end is a circular marker used as the
// insertion point for append and as the boundary returned by edge traversal.
typedef struct
{
    ralc_iface *parentRalc;
    void *end;    // End marker; NULL before the first successful insertion.
    size_t count; // Live element count, excluding the end marker.
    ralc_iface ifaceRalc;
} rlist;

// Initializes an empty list.
void rlist_init(rlist *p, ralc_iface *parentRalc_p);

// Inserts before obj_p. Pass p->end to append; on an empty list, pass NULL.
void *rlist_insert(rlist *p, const void *obj_p, size_t size);
// Resizes a live element. The return value is the current pointer to that
// element because resizing can move it. obj_p must not be p->end.
void *rlist_resize(rlist *p, void *obj_p, size_t newSize);
// Removes count live elements starting at obj_p. obj_p may be p->end only when
// count is zero. Returns the following element, p->end, or NULL when the list
// became empty.
void *rlist_remove(rlist *p, void *obj_p, size_t count);

// Returns the previous payload pointer in the circular list. At the front edge,
// this returns p->end. obj_p may be p->end.
void *rlist_prev(const void *obj_p);
// Returns the next payload pointer in the circular list. At the back edge, this
// returns p->end. obj_p may be p->end.
void *rlist_next(const void *obj_p);
// Returns the backing allocator request size for one list element payload.
size_t rlist_elemAllocSize(size_t objSize);
// Returns the actual payload size of a live element. obj_p must not be p->end.
size_t rlist_elemSize(const void *obj_p);

// Returns obj_p's zero-based index. obj_p must be a live element in p, not
// p->end.
size_t rlist_index(rlist *p, const void *obj_p);
// Returns the element at index. index must be less than p->count.
void *rlist_at(rlist *p, size_t index);

// Frees every node, including the end marker, and leaves p empty.
void rlist_clear(rlist *p);

// Fills out_usage with parent allocator accounting adjusted so list headers and
// the end marker are system storage rather than user-visible blocks.
void rlist_getUsage(rlist *p, ralc_usage *out_usage);

// Returns the first live element, or NULL when the list is empty.
#define rlist_first(_list_p)            ((_list_p)->end ? rlist_next((_list_p)->end) : NULL)
// Returns the last live element, or NULL when the list is empty.
#define rlist_last(_list_p)             ((_list_p)->end ? rlist_prev((_list_p)->end) : NULL)
// Appends one element and returns its payload pointer.
#define rlist_push(_list_p, _size)      rlist_insert(_list_p, (_list_p)->end, _size)
// Prepends one element and returns its payload pointer.
#define rlist_pushFront(_list_p, _size) rlist_insert(_list_p, rlist_first(_list_p), _size)

#endif
