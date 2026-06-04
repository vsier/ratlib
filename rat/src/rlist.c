// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rlist.h>
#include <rat/extra.h>
#include <assert.h>
#include <string.h>


typedef struct elem
{
    struct elem *prev;
    struct elem *next;
    unsigned char data[];
} elem;

// The parent allocator sees node header + payload as one block; public list
// pointers always point at data.
#define HEADER_SIZE RAT_ALIGN(sizeof(elem))


void rlist_init(rlist *p, ralc_iface *parentRalc_p)
{
    assert(p && parentRalc_p);

    p->parentRalc = parentRalc_p;
    p->first = NULL;
    p->last = NULL;
    p->count = 0;
    p->ifaceRalc.kind = RALC_LIST;
}

size_t rlist_elemAllocSize(size_t objSize)
{
    assert(objSize <= SIZE_MAX - HEADER_SIZE);
    return HEADER_SIZE + objSize;
}

void *rlist_insert(rlist *p, const void *obj_p, size_t size, size_t *out_actualSize)
{
    assert(p && p->parentRalc && out_actualSize);

    size_t requestedSize = rlist_elemAllocSize(size);
    unsigned char *block_p = ralc_alloc(p->parentRalc, requestedSize, &size);
    if (!block_p) return NULL;
    assert(size >= requestedSize);

    elem *restrict elem_p = (elem *)(block_p + HEADER_SIZE) - 1;

    elem *restrict prev_p, *restrict next_p;
    if (obj_p)
    {
        next_p = (elem *)obj_p - 1;
        prev_p = next_p->prev;

        next_p->prev = elem_p;
    }
    else
    {
        next_p = NULL;
        prev_p = p->last ? (elem *)p->last - 1 : NULL;

        p->last = elem_p->data;
    }

    if (prev_p)
        prev_p->next = elem_p;
    else
        p->first = elem_p->data;

    elem_p->prev = prev_p;
    elem_p->next = next_p;
    ++p->count;

    *out_actualSize = size - HEADER_SIZE;
    return elem_p->data;
}

void *rlist_resize(rlist *p, void *obj_p, size_t newSize, size_t *out_actualNewSize)
{
    assert(p && p->parentRalc && out_actualNewSize);

    if (!obj_p) return rlist_insert(p, NULL, newSize, out_actualNewSize);

    unsigned char *restrict block_p = (unsigned char *)obj_p - HEADER_SIZE;

    size_t requestedSize = rlist_elemAllocSize(newSize);
    unsigned char *restrict newBlock_p = ralc_realloc(p->parentRalc, block_p, requestedSize, &newSize);
    if (!newBlock_p) return NULL;
    assert(newSize >= requestedSize);

    elem *restrict newElem_p = (elem *)(newBlock_p + HEADER_SIZE) - 1;

    if (newBlock_p != block_p)
    {
        if (newElem_p->prev)
            newElem_p->prev->next = newElem_p;
        else
            p->first = newElem_p->data;

        if (newElem_p->next)
            newElem_p->next->prev = newElem_p;
        else
            p->last = newElem_p->data;
    }

    *out_actualNewSize = newSize - HEADER_SIZE;
    return newElem_p->data;
}

void *rlist_remove(rlist *p, void *obj_p, size_t count)
{
    assert(p && p->parentRalc && obj_p && count && count <= p->count);

    elem *elem_p = (elem *)obj_p - 1;
    elem *restrict const before_p = elem_p->prev;

    while (count--)
    {
        unsigned char *block_p = elem_p->data - HEADER_SIZE;
        elem *restrict const next_p = elem_p->next;

        ralc_free(p->parentRalc, block_p);
        --p->count;

        elem_p = next_p;
    }

    unsigned char *restrict elemData_p = elem_p ? elem_p->data : NULL;
    unsigned char *restrict beforeData_p = before_p ? before_p->data : NULL;

    if (before_p)
        before_p->next = elem_p;
    else
        p->first = elemData_p;

    if (elem_p)
        elem_p->prev = before_p;
    else
        p->last = beforeData_p;

    return elemData_p;
}


void rlist_clear(rlist *p)
{
    assert(p && ((p->count && p->first && p->last) || (!p->count && !p->first && !p->last)));

    if (p->count) rlist_remove(p, p->first, p->count);
}


void *rlist_prev(const void *obj_p)
{
    assert(obj_p);

    const elem *elem_p = (const elem *)obj_p - 1;
    return elem_p->prev ? elem_p->prev + 1 : NULL;
}

void *rlist_next(const void *obj_p)
{
    assert(obj_p);

    const elem *elem_p = (const elem *)obj_p - 1;
    return elem_p->next ? elem_p->next + 1 : NULL;
}


size_t rlist_index(rlist *p, const void *obj_p)
{
    assert(p && obj_p && p->count);

    // Walk both directions and stop at whichever end is closer.
    const elem *restrict right_p, *restrict left_p;
    {
        const elem *curr_p = (const elem *)obj_p - 1;
        right_p = curr_p->next;
        left_p = curr_p->prev;
    }

    size_t index = 0;
    while (true)
    {
        if (!left_p) return index;
        if (!right_p) return p->count - 1 - index;

        right_p = right_p->next;
        left_p = left_p->prev;
        ++index;
    }
}

void *rlist_at(rlist *p, size_t index)
{
    assert(p && index < p->count);

    const elem *restrict curr_p;

    if (index < p->count >> 1)
    {
        curr_p = (elem *)p->first - 1;
        while (index--) curr_p = curr_p->next;
    }
    else
    {
        index = p->count - index - 1;

        curr_p = (elem *)p->last - 1;
        while (index--) curr_p = curr_p->prev;
    }

    return (void *)curr_p->data;
}


void rlist_getUsage(rlist *p, ralc_usage *out_usage)
{
    assert(p && out_usage);

    ralc_getUsage(p->parentRalc, out_usage);

    assert(p->count <= SIZE_MAX / HEADER_SIZE);
    size_t sysSize = p->count * HEADER_SIZE;
    assert(out_usage->blockSize >= sysSize);

    out_usage->systemSize += sysSize;
    out_usage->blockSize -= sysSize;
}
