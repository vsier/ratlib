// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include <rat/rlist.h>
#include <rat/extra.h>
#include <assert.h>
#include <string.h>


typedef struct elem
{
    struct elem *prev;
    struct elem *next;
    size_t size;
    unsigned char data[];
} elem;

// The parent allocator sees node header + payload as one block; public list
// pointers always point at data.
#define HEADER_SIZE RAT_ALIGN(sizeof(elem))
#define OBJ_ELEM(_obj_p)    ((elem *)(_obj_p) - 1)
#define OBJ_BLOCK(_obj_p)   ((unsigned char *)(_obj_p) - HEADER_SIZE)
#define BLOCK_OBJ(_block_p) ((_block_p) + HEADER_SIZE)


void rlist_init(rlist *p, ralc_iface *parentRalc_p)
{
    assert(p && parentRalc_p);

    p->parentRalc = parentRalc_p;
    p->first = NULL;
    p->last = NULL;
    p->count = 0;
    p->ifaceRalc.kind = RALC_LIST;
}

void *rlist_insert(rlist *p, const void *obj_p, size_t size)
{
    assert(p && p->parentRalc && (!obj_p || p->count));

    unsigned char *restrict block_p = ralc_alloc(p->parentRalc, rlist_elemAllocSize(size), &size);
    if (!block_p) return NULL;
    assert(size >= HEADER_SIZE);

    elem *restrict elem_p = OBJ_ELEM(BLOCK_OBJ(block_p));
    elem *restrict prev_p;
    elem *restrict next_p;
    if (obj_p)
    {
        next_p = OBJ_ELEM(obj_p);
        prev_p = next_p->prev;
        next_p->prev = elem_p;
    }
    else
    {
        next_p = NULL;
        prev_p = p->last ? OBJ_ELEM(p->last) : NULL;
        p->last = elem_p->data;
    }

    if (prev_p)
        prev_p->next = elem_p;
    else
        p->first = elem_p->data;

    elem_p->prev = prev_p;
    elem_p->next = next_p;
    elem_p->size = size - HEADER_SIZE;

    ++p->count;
    return elem_p->data;
}

void *rlist_resize(rlist *p, void *obj_p, size_t newSize)
{
    assert(p && p->parentRalc && obj_p);

    unsigned char *restrict block_p = OBJ_BLOCK(obj_p);
    size_t blockSize = HEADER_SIZE + OBJ_ELEM(obj_p)->size;
    unsigned char *restrict newBlock_p = ralc_realloc(p->parentRalc, block_p, rlist_elemAllocSize(newSize), &newSize);
    if (!newBlock_p)
    {
        newBlock_p = ralc_alloc(p->parentRalc, rlist_elemAllocSize(newSize), &newSize);
        if (!newBlock_p) return NULL;
        assert(newSize >= HEADER_SIZE);

        memcpy(newBlock_p, block_p, blockSize < newSize ? blockSize : newSize);
        ralc_free(p->parentRalc, block_p);
    }
    assert(newSize >= HEADER_SIZE);

    elem *restrict newElem_p = OBJ_ELEM(BLOCK_OBJ(newBlock_p));
    if (newElem_p->prev)
        newElem_p->prev->next = newElem_p;
    else
        p->first = newElem_p->data;

    if (newElem_p->next)
        newElem_p->next->prev = newElem_p;
    else
        p->last = newElem_p->data;

    newElem_p->size = newSize - HEADER_SIZE;
    return newElem_p->data;
}

void *rlist_remove(rlist *p, void *obj_p, size_t count)
{
    assert(p && p->parentRalc && count <= p->count &&
        ((obj_p && count) || (!obj_p && !count)));
    if (!count) return NULL;

    elem *elem_p = OBJ_ELEM(obj_p);
    elem *restrict const before_p = elem_p->prev;
    for (size_t i = 0; i < count; ++i)
    {
        elem *next_p = elem_p->next;
        ralc_free(p->parentRalc, OBJ_BLOCK(elem_p->data));
        elem_p = next_p;
    }

    if (before_p)
        before_p->next = elem_p;
    else
        p->first = elem_p ? elem_p->data : NULL;

    if (elem_p)
        elem_p->prev = before_p;
    else
        p->last = before_p ? before_p->data : NULL;

    p->count -= count;
    return elem_p ? elem_p->data : NULL;
}


void *rlist_prev(const void *obj_p)
{
    assert(obj_p);

    elem *elem_p = OBJ_ELEM(obj_p);
    return elem_p->prev ? elem_p->prev->data : NULL;
}
void *rlist_next(const void *obj_p)
{
    assert(obj_p);

    elem *elem_p = OBJ_ELEM(obj_p);
    return elem_p->next ? elem_p->next->data : NULL;
}

size_t rlist_elemAllocSize(size_t objSize)
{
    assert(objSize <= SIZE_MAX - HEADER_SIZE);
    return HEADER_SIZE + objSize;
}
size_t rlist_elemSize(const void *obj_p)
{
    assert(obj_p);

    return OBJ_ELEM(obj_p)->size;
}


size_t rlist_index(rlist *p, const void *obj_p)
{
    assert(p && p->count && obj_p);

    const elem *elem_p = OBJ_ELEM(p->first);
    const elem *target_p = OBJ_ELEM(obj_p);

    size_t index = 0;
    for (; elem_p != target_p; elem_p = elem_p->next)
        ++index;

    return index;
}

void *rlist_at(rlist *p, size_t index)
{
    assert(p && index < p->count);

    elem *restrict curr_p;
    if (index < p->count >> 1)
    {
        curr_p = OBJ_ELEM(p->first);
        while (index--) curr_p = curr_p->next;
    }
    else
    {
        index = p->count - index - 1;

        curr_p = OBJ_ELEM(p->last);
        while (index--) curr_p = curr_p->prev;
    }

    return curr_p->data;
}


void rlist_getUsage(rlist *p, ralc_usage *out_usage)
{
    assert(p && p->parentRalc && out_usage);

    ralc_getUsage(p->parentRalc, out_usage);

    assert(p->count <= SIZE_MAX / HEADER_SIZE);
    size_t sysSize = p->count * HEADER_SIZE;
    assert(out_usage->systemSize <= SIZE_MAX - sysSize);
    assert(out_usage->blockSize >= sysSize);

    out_usage->systemSize += sysSize;
    out_usage->blockSize -= sysSize;
}
