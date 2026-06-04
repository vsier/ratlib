// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/ralcPool.h>
#include <rat/extra.h>
#include <assert.h>


typedef struct chunk
{
    struct chunk *next;
    size_t cap;
    size_t blockSize;
    unsigned char data[];
} chunk;

typedef struct freeSlot
{
    struct freeSlot *next;
} freeSlot;

// Free slots store the next pointer in user storage, so freeing a slot destroys
// its previous contents.
#define HEADER_SIZE RAT_ALIGN(sizeof(chunk))
#define CHUNK_BLOCK(_chunk_p) ((unsigned char *)((_chunk_p)->data - HEADER_SIZE))
#define BLOCK_CHUNK(_block_p) ((chunk *)((unsigned char *)(_block_p) + HEADER_SIZE) - 1)


static bool grow(ralcPool *p)
{
    assert(p && p->parentRalc && p->elemSize >= sizeof(freeSlot) && p->chunkCap > 0 &&
        p->chunkCap <= (SIZE_MAX - HEADER_SIZE) / p->elemSize);

    size_t blockSize = HEADER_SIZE + p->chunkCap * p->elemSize;

#ifndef NDEBUG
    size_t requestedBlockSize = blockSize;
#endif
    unsigned char *block_p = ralc_alloc(p->parentRalc, blockSize, &blockSize);
    if (!block_p) return false;
    assert(blockSize >= requestedBlockSize);

    chunk *restrict chunk_p = BLOCK_CHUNK(block_p);
    chunk_p->next = p->chunks;
    chunk_p->cap = (blockSize - HEADER_SIZE) / p->elemSize;
    chunk_p->blockSize = blockSize;

    p->chunks = chunk_p;

    for (size_t i = 0; i < chunk_p->cap; ++i)
    {
        freeSlot *restrict slot_p = (freeSlot *)(chunk_p->data + i * p->elemSize);
        slot_p->next = p->freeSlots;
        p->freeSlots = slot_p;
    }

    return true;
}


void ralcPool_init(ralcPool *p, size_t elemSize, size_t initChunkCap, ralc_iface *parentRalc_p)
{
    assert(p && parentRalc_p && RAT_ALIGNMENT > 0);

    elemSize = elemSize > sizeof(freeSlot) ? elemSize : sizeof(freeSlot);
    assert(elemSize <= SIZE_MAX - (RAT_ALIGNMENT - 1));
    elemSize = RAT_ALIGN(elemSize);

    p->parentRalc = parentRalc_p;
    p->chunks = NULL;
    p->freeSlots = NULL;
    p->elemSize = elemSize;
    p->chunkCap = initChunkCap ? initChunkCap : 1;
    p->count = 0;
    p->ifaceRalc.kind = RALC_POOL;
}

void *ralcPool_alloc(ralcPool *p)
{
    assert(p);

    if (!p->freeSlots && !grow(p))
        return NULL;

    freeSlot *restrict slot_p = p->freeSlots;
    p->freeSlots = slot_p->next;
    ++p->count;

    return slot_p;
}

void ralcPool_free(ralcPool *p, void *ptr)
{
    assert(p && ptr && p->count > 0);

    freeSlot *restrict slot_p = ptr;
    slot_p->next = p->freeSlots;
    p->freeSlots = slot_p;
    --p->count;
}


void ralcPool_clear(ralcPool *p)
{
    assert(p && p->parentRalc);

    chunk *next_p;
    for (chunk *chunk_p = p->chunks; chunk_p; chunk_p = next_p)
    {
        next_p = chunk_p->next;
        ralc_free(p->parentRalc, CHUNK_BLOCK(chunk_p));
    }

    p->chunks = NULL;
    p->freeSlots = NULL;
    p->count = 0;
}


void ralcPool_getUsage(ralcPool *p, ralc_usage *out_usage)
{
    assert(p && p->parentRalc && out_usage);

    ralc_getUsage(p->parentRalc, out_usage);

    // Total capacity is derived here instead of stored in ralcPool; allocation
    // only needs the free-list head.
    size_t chunkCount = 0;
    size_t cap = 0;
    size_t systemSize = 0;
    for (chunk *chunk_p = p->chunks; chunk_p; chunk_p = chunk_p->next)
    {
        ++chunkCount;
        assert(chunk_p->cap <= chunk_p->blockSize / p->elemSize);
        cap += chunk_p->cap;
        systemSize += chunk_p->blockSize - chunk_p->cap * p->elemSize;
    }

    assert(cap >= p->count);

    size_t holeCount = cap - p->count;
    size_t holeSize = holeCount * p->elemSize;

    out_usage->blockCount -= chunkCount;
    out_usage->blockCount += p->count;
    out_usage->blockSize -= systemSize + holeSize;
    out_usage->holeCount += holeCount;
    out_usage->holeSize += holeSize;
    out_usage->systemSize += systemSize;
}
