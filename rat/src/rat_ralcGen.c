// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/ralcGen.h>
#include <rat/extra.h>
#include <assert.h>
#include <string.h>


typedef struct
{
    size_t prevSize_unused;
    size_t size;
    unsigned char data[];
} block;


// Blocks are packed from p->buf to p->buf + p->size. prevSize_unused stores the
// previous payload size plus the current block's unused flag in the low bit;
// aligned payload sizes leave that bit available.
#define MASK_UNUSED   ((size_t)0x01)
#define MASK_PREVSIZE (~MASK_UNUSED)

#define BLOCK_PREV(_block)      ((block *)((unsigned char *)((_block) - 1) - ((_block)->prevSize_unused & MASK_PREVSIZE)))
#define BLOCK_NEXT(_block)      ((block *)((_block)->data + (_block)->size))
#define BLOCK_HAS_PREV(_block)  ((const unsigned char *)(_block) > p->buf)
#define BLOCK_IS_END(_block)    ((const unsigned char *)(_block) >= p->buf + p->size)
#define BLOCK_IS_UNUSED(_block) ((_block)->prevSize_unused & MASK_UNUSED)


static void block_setSize(ralcGen *p, block *block_p, size_t size)
{
    assert(p && block_p);

    block_p->size = size;

    block *next_p = BLOCK_NEXT(block_p);
    if (BLOCK_IS_END(next_p)) return;

    next_p->prevSize_unused = size | BLOCK_IS_UNUSED(next_p);
}
static size_t block_split(ralcGen *p, block *block_p, size_t availableSize, size_t usedSize)
{
    assert(p && block_p && usedSize <= availableSize);

    size_t remainingSize = availableSize - usedSize;

    // If the tail cannot hold another header, give the caller the whole block.
    if (remainingSize >= sizeof(block))
    {
        block *next_p = (block *)(block_p->data + usedSize);
        block_setSize(p, next_p, remainingSize - sizeof(*next_p));
        next_p->prevSize_unused = MASK_UNUSED;
    }
    else
    {
        usedSize = availableSize;
    }

    block_setSize(p, block_p, usedSize);
    block_p->prevSize_unused &= MASK_PREVSIZE;
    return usedSize;
}
static void block_merge(ralcGen *p, block *block_p)
{
    assert(p && block_p);

    size_t totalSize = block_p->size;

    // Merge with adjacent unused blocks. If the merged block reaches the end of
    // the used buffer, shrink p->size instead of leaving a trailing hole.
    {
        size_t addedSize = 0;
        if (BLOCK_HAS_PREV(block_p))
        {
            block *prev_p = BLOCK_PREV(block_p);
            addedSize += BLOCK_IS_UNUSED(prev_p) ? prev_p->size + sizeof(*block_p) : 0;
        }
        totalSize += addedSize;
        block_p = (block *)((unsigned char *)block_p - addedSize);
    }

    {
        block *next_p = (block *)(block_p->data + totalSize);
        if (BLOCK_IS_END(next_p))
        {
            p->size -= sizeof(*block_p) + totalSize;
            return;
        }
        totalSize += BLOCK_IS_UNUSED(next_p) ? (sizeof(*next_p) + next_p->size) : 0;
    }

    block_setSize(p, block_p, totalSize);
    block_p->prevSize_unused |= MASK_UNUSED;
}


void ralcGen_init(ralcGen *p, void *buf, size_t bufSize)
{
    assert(p && buf);

    p->buf = buf;
    p->cap = bufSize;
    p->size = 0;
    p->ifaceRalc.kind = RALC_GENERAL;
}

void *ralcGen_alloc(ralcGen *p, size_t size, size_t *out_actualSize)
{
    assert(p && p->buf && p->size <= p->cap && out_actualSize);
    assert(RAT_ALIGNMENT > 0 && size <= SIZE_MAX - (RAT_ALIGNMENT - 1));

    size = RAT_ALIGN(size);

    size_t prevSize = 0;
    block *block_p = (block *)p->buf;
    while (!BLOCK_IS_END(block_p))
    {
        if (BLOCK_IS_UNUSED(block_p) && block_p->size >= size)
        {
            size = block_split(p, block_p, block_p->size, size);
            goto end;
        }

        prevSize = block_p->size;
        block_p = BLOCK_NEXT(block_p);
    }

    if (p->cap - p->size < sizeof(block) + size)
        return NULL;

    p->size += sizeof(*block_p) + size;
    block_p->prevSize_unused = prevSize;
    block_p->size = size;

end:
    *out_actualSize = size;
    return block_p->data;
}

void *ralcGen_realloc(ralcGen *p, void *ptr, size_t newSize, size_t *out_actualNewSize)
{
    assert(p && p->buf && p->size <= p->cap && out_actualNewSize);
    assert(RAT_ALIGNMENT > 0 && newSize <= SIZE_MAX - (RAT_ALIGNMENT - 1));

    if (!ptr) return ralcGen_alloc(p, newSize, out_actualNewSize);

    newSize = RAT_ALIGN(newSize);

    block *restrict const block_p = (block *)ptr - 1;
    block *restrict const next_p = BLOCK_NEXT(block_p);
    size_t availableSize = block_p->size;

    if (BLOCK_IS_END(next_p))
    {
        size_t newBufSize = p->size - block_p->size + newSize;
        if (newBufSize <= p->cap)
        {
            p->size = newBufSize;
            block_p->size = newSize;
            goto end;
        }
    }
    else
    {
        availableSize += BLOCK_IS_UNUSED(next_p) ? (sizeof(*next_p) + next_p->size) : 0;
    }

    if (availableSize >= newSize)
    {
        newSize = block_split(p, block_p, availableSize, newSize);
        goto end;
    }

    void *restrict newObj_p = ralcGen_alloc(p, newSize, out_actualNewSize);
    if (newObj_p)
    {
        memcpy(newObj_p, ptr, block_p->size);
        block_merge(p, block_p);
    }
    return newObj_p;

end:
    *out_actualNewSize = newSize;
    return block_p->data;
}

void ralcGen_free(ralcGen *p, void *ptr)
{
    assert(p && p->buf && ptr);

    block_merge(p, (block *)ptr - 1);
}


void ralcGen_clear(ralcGen *p)
{
    assert(p && p->buf);

    p->size = 0;
}


void ralcGen_getUsage(ralcGen *p, ralc_usage *out_usage)
{
    assert(p && p->buf && p->size <= p->cap && out_usage);

    memset(out_usage, 0, sizeof(*out_usage));

    block *block_p = (block *)p->buf;
    while (!BLOCK_IS_END(block_p))
    {
        size_t isUnused = BLOCK_IS_UNUSED(block_p);
        size_t isUnusedMask = ~isUnused + 1;
        out_usage->blockCount += !isUnused;
        out_usage->blockSize += ~isUnusedMask & block_p->size;
        out_usage->holeCount += isUnused;
        out_usage->holeSize += isUnusedMask & block_p->size;

        block_p = BLOCK_NEXT(block_p);
    }

    size_t remainingSize = p->cap - p->size;
    out_usage->holeSize += (remainingSize > sizeof(block)) * (remainingSize - sizeof(block));
    out_usage->systemSize = p->cap - out_usage->blockSize - out_usage->holeSize;
}
