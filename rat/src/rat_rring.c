// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rring.h>
#include <assert.h>


void rring_init(rring *p, void *buf, size_t elemSize, size_t cap)
{
    assert(p && elemSize && (buf || !cap));

    p->buf = buf;
    p->elemSize = elemSize;
    p->cap = cap;
    p->first = 0;
    p->count = 0;
}

void rring_clear(rring *p)
{
    assert(p);

    p->first = 0;
    p->count = 0;
}

void *rring_push(rring *p)
{
    assert(p);

    if (p->count == p->cap)
        return NULL;

    size_t index = p->first + p->count;
    if (p->cap <= index)
        index -= p->cap;

    ++p->count;
    return p->buf + index * p->elemSize;
}

void *rring_pushFront(rring *p)
{
    assert(p);

    if (p->count == p->cap)
        return NULL;

    p->first = p->first ? p->first - 1 : p->cap - 1;
    ++p->count;
    return p->buf + p->first * p->elemSize;
}

void rring_pop(rring *p, size_t count)
{
    assert(p && count <= p->count);

    p->count -= count;
}

void rring_popFront(rring *p, size_t count)
{
    assert(p && count <= p->count);

    p->first += count;
    if (p->cap <= p->first)
        p->first -= p->cap;
    p->count -= count;
}

void *rring_at(const rring *p, size_t index)
{
    assert(p && index < p->count);

    index += p->first;
    if (p->cap <= index)
        index -= p->cap;

    return p->buf + index * p->elemSize;
}

void *rring_last(const rring *p, size_t index)
{
    assert(p && index < p->count);

    index = p->first + p->count - 1 - index;
    if (p->cap <= index)
        index -= p->cap;

    return p->buf + index * p->elemSize;
}
