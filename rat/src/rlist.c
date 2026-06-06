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
    p->end = NULL;
    p->count = 0;
    p->ifaceRalc.kind = RALC_LIST;
}

void *rlist_insert(rlist *p, const void *obj_p, size_t size)
{
    assert(p && p->parentRalc && ((p->end && obj_p) || (!p->end && !obj_p)));

    if (!p->end)
    {
        size_t endSize = rlist_elemAllocSize(0);
        unsigned char *endBlock_p = ralc_alloc(p->parentRalc, endSize, &endSize);
        if (!endBlock_p) return NULL;

        elem *end_p = OBJ_ELEM(BLOCK_OBJ(endBlock_p));
        end_p->next = end_p->prev = end_p;
        end_p->size = endSize - HEADER_SIZE;

        obj_p = p->end = end_p->data;
    }

    unsigned char *restrict block_p = ralc_alloc(p->parentRalc, rlist_elemAllocSize(size), &size);
    if (!block_p)
    {
        if (!p->count)
        {
            ralc_free(p->parentRalc, OBJ_BLOCK(p->end));
            p->end = NULL;
        }
        return NULL;
    }

    elem *restrict elem_p = OBJ_ELEM(BLOCK_OBJ(block_p));
    elem_p->next = OBJ_ELEM(obj_p);
    elem_p->prev = elem_p->next->prev;
    elem_p->size = size - HEADER_SIZE;
    elem_p->next->prev = elem_p->prev->next = elem_p;

    ++p->count;
    return elem_p->data;
}

void *rlist_resize(rlist *p, void *obj_p, size_t newSize)
{
    assert(p && p->parentRalc && p->end && obj_p && obj_p != p->end);

    unsigned char *restrict block_p = OBJ_BLOCK(obj_p);
    unsigned char *restrict newBlock_p = ralc_realloc(p->parentRalc, block_p, rlist_elemAllocSize(newSize), &newSize);
    if (!newBlock_p)
    {
        size_t blockSize = HEADER_SIZE + OBJ_ELEM(obj_p)->size;
        newBlock_p = ralc_alloc(p->parentRalc, rlist_elemAllocSize(newSize), &newSize);
        if (!newBlock_p) return NULL;
        memcpy(newBlock_p, block_p, blockSize < newSize ? blockSize : newSize);
        ralc_free(p->parentRalc, block_p);
    }

    elem *restrict newElem_p = OBJ_ELEM(BLOCK_OBJ(newBlock_p));
    newElem_p->next->prev = newElem_p->prev->next = newElem_p;
    newElem_p->size = newSize - HEADER_SIZE;

    return newElem_p->data;
}

void *rlist_remove(rlist *p, void *obj_p, size_t count)
{
    assert(p && p->parentRalc && count <= p->count &&
        ((!p->end && !obj_p && !count) ||
        (p->end && obj_p && (obj_p != p->end || !count) &&
        (count < p->count || obj_p == rlist_first(p)))));
    if (count == p->count)
    {
        rlist_clear(p);
        return NULL;
    }

    elem *elem_p = OBJ_ELEM(obj_p);
    elem *restrict const before_p = elem_p->prev;
    for (size_t i = 0; i < count; ++i)
    {
        elem *next_p = elem_p->next;
        ralc_free(p->parentRalc, OBJ_BLOCK(elem_p->data));
        elem_p = next_p;
    }

    elem_p->prev = before_p;
    before_p->next = elem_p;

    p->count -= count;
    return elem_p->data;
}


void *rlist_prev(const void *obj_p)
{
    assert(obj_p);
    return OBJ_ELEM(obj_p)->prev->data;
}
void *rlist_next(const void *obj_p)
{
    assert(obj_p);
    return OBJ_ELEM(obj_p)->next->data;
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
    assert(p && p->end && obj_p && obj_p != p->end && p->count);

    const elem *elem_p = OBJ_ELEM(p->end)->next;
    const elem *target_p = OBJ_ELEM(obj_p);

    size_t index = 0;
    for (; elem_p != target_p; elem_p = elem_p->next)
        ++index;

    return index;
}

void *rlist_at(rlist *p, size_t index)
{
    assert(p && p->end && index < p->count);

    elem *restrict curr_p;
    if (index < p->count >> 1)
    {
        curr_p = OBJ_ELEM(p->end)->next;
        while (index--) curr_p = curr_p->next;
    }
    else
    {
        index = p->count - index - 1;

        curr_p = OBJ_ELEM(p->end)->prev;
        while (index--) curr_p = curr_p->prev;
    }

    return curr_p->data;
}


void rlist_clear(rlist *p)
{
    assert(p && p->parentRalc);

    if (!p->end) return;

    elem *end_p = OBJ_ELEM(p->end);
    for (elem *next_p, *elem_p = end_p->next;; elem_p = next_p)
    {
        next_p = elem_p->next;
        ralc_free(p->parentRalc, OBJ_BLOCK(elem_p->data));
        if (elem_p == end_p) break;
    }
    p->end = NULL;
    p->count = 0;
}


void rlist_getUsage(rlist *p, ralc_usage *out_usage)
{
    assert(p && p->parentRalc && out_usage);

    ralc_getUsage(p->parentRalc, out_usage);

    assert(p->count <= SIZE_MAX / HEADER_SIZE);
    size_t sysSize = p->count * HEADER_SIZE;
    if (p->end)
    {
        size_t endSize = OBJ_ELEM(p->end)->size;
        assert(sysSize <= SIZE_MAX - HEADER_SIZE - endSize);
        assert(out_usage->blockCount > 0);

        sysSize += HEADER_SIZE + endSize;
        --out_usage->blockCount;
    }
    assert(out_usage->systemSize <= SIZE_MAX - sysSize);
    assert(out_usage->blockSize >= sysSize);

    out_usage->systemSize += sysSize;
    out_usage->blockSize -= sysSize;
}
