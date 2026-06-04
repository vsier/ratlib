// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rdyn.h>
#include <rat/rarr.h>
#include <assert.h>
#include <string.h>


static unsigned char *reallocBuf(rdyn *p, size_t cap, size_t copyCount, size_t *out_actualCap)
{
    assert(p && p->ralc && p->elemSize && copyCount <= p->count &&
        copyCount <= cap && cap <= SIZE_MAX / p->elemSize && out_actualCap);

    size_t actualSize;
    size_t size = cap * p->elemSize;
    unsigned char *newArr = ralc_realloc(p->ralc, p->buf, size, &actualSize);
    if (!newArr)
    {
        // Some allocators are alloc-only through the generic interface. Fall
        // back to allocate-copy-free so rdyn can still use them.
        newArr = ralc_alloc(p->ralc, size, &actualSize);
        if (!newArr) return NULL;

        // Project contract: zero-byte memcpy with NULL pointers is acceptable.
        memcpy(newArr, p->buf, copyCount * p->elemSize);
        if (p->buf) ralc_free(p->ralc, p->buf);
    }
    assert(actualSize >= size);

    *out_actualCap = actualSize / p->elemSize;

    return newArr;
}

static bool grow(rdyn *p, size_t growCount)
{
    assert(p && growCount <= SIZE_MAX - p->count);

    size_t newCount = p->count + growCount;
    if (newCount <= p->cap)
    {
        p->count = newCount;
        return true;
    }

    size_t newCap = newCount + newCount / 2;
    unsigned char *newArr = reallocBuf(p, newCap, p->count, &newCap);
    if (!newArr)
    {
        newCap = newCount;
        newArr = reallocBuf(p, newCap, p->count, &newCap);
        if (!newArr) return false;
    }

    p->buf = newArr;
    p->count = newCount;
    p->cap = newCap;
    return true;
}

static void shrink(rdyn *p, size_t shrinkCount)
{
    assert(p && shrinkCount <= p->count);

    if (!p->allowShrink)
    {
        p->count -= shrinkCount;
        return;
    }

    if (p->count <= shrinkCount)
    {
        rdyn_clear(p);
        return;
    }

    const size_t newCount = p->count - shrinkCount;
    if (newCount <= p->cap / 2)
    {
        size_t newCap = newCount + newCount / 3;
        unsigned char *newArr = reallocBuf(p, newCap, newCount, &newCap);
        if (newArr)
        {
            p->buf = newArr;
            p->cap = newCap;
        }
    }

    p->count = newCount;
}


void rdyn_init(rdyn *p, size_t elemSize, ralc_iface *ralc_if)
{
    assert(p && elemSize && ralc_if);

    p->ralc = ralc_if;
    p->buf = NULL;
    p->elemSize = elemSize;
    p->cap = 0;
    p->count = 0;
    p->allowShrink = false;
}


void *rdyn_insert(rdyn *p, size_t index, size_t count)
{
    assert(p && index <= p->count && count <= SIZE_MAX - p->count);

    if (!grow(p, count))
        return NULL;

    return rarr_insert(p->buf, p->elemSize, p->count, index, count);
}

void *rdyn_resize(rdyn *p, size_t index, size_t oldCount, size_t newCount)
{
    assert(p && index <= p->count && oldCount <= p->count - index);

    void *result;
    if (newCount > oldCount)
        result = rdyn_insert(p, index + oldCount, newCount - oldCount);
    else
        result = rdyn_remove(p, index + newCount, oldCount - newCount);

    return result ? p->buf + index * p->elemSize : NULL;
}

void *rdyn_remove(rdyn *p, size_t index, size_t count)
{
    assert(p && index <= p->count && count <= p->count - index);

    rarr_remove(p->buf, p->elemSize, p->count, index, count);
    shrink(p, count);
    return p->buf ? p->buf + index * p->elemSize : NULL;
}


bool rdyn_setCap(rdyn *p, size_t newCap)
{
    assert(p);

    if (!newCap)
    {
        rdyn_clear(p);
        return true;
    }

    size_t copyCount = p->count < newCap ? p->count : newCap;
    unsigned char *newArr = reallocBuf(p, newCap, copyCount, &newCap);
    if (!newArr) return false;

    p->buf = newArr;
    p->cap = newCap;
    if (p->count > p->cap) p->count = p->cap;
    return true;
}

void rdyn_clear(rdyn *p)
{
    assert(p);

    if (p->buf) ralc_free(p->ralc, p->buf);
    p->buf = NULL;
    p->count = 0;
    p->cap = 0;
}
