// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include <rat/ralcArena.h>
#include <rat/extra.h>
#include <assert.h>
#include <string.h>


// Segment names:
//           __________________block__________________
//          /           _____________chunk____________\
//         /           /                               \
//        +-----------+-----------+---------------------+
// Data:  |  padding  |  header   |      user data      |
//        +-----------+-----------+---------------------+
// Sizes:              <-sizeof-->
//         <-----HEADER_SIZE-----> <--RAT_ALIGN(size)-->


typedef struct chunk
{
    struct chunk *next;
    size_t cap;
    size_t size;
    unsigned char data[];
} chunk;

#define HEADER_SIZE RAT_ALIGN(sizeof(chunk))
#define CHUNK_BLOCK(_chunk_p) ((unsigned char *)((_chunk_p)->data - HEADER_SIZE))
#define BLOCK_CHUNK(_block_p) ((chunk *)((unsigned char *)(_block_p) + HEADER_SIZE) - 1)


// p->chunks is a stack; new allocations use the top chunk when it has room.
void ralcArena_init(ralcArena *p, size_t initChunkCap, ralc_iface *parentRalc_p)
{
    assert(p && parentRalc_p);

    p->parentRalc = parentRalc_p;
    p->chunks = NULL;
    p->chunkCap = initChunkCap;
    p->size = 0;
    p->ifaceRalc.kind = RALC_ARENA;
}

void *ralcArena_push(ralcArena *p, size_t size, size_t *out_actualSize)
{
    assert(p && p->parentRalc && out_actualSize);
    assert(RAT_ALIGNMENT > 0 && size <= SIZE_MAX - (RAT_ALIGNMENT - 1));

    void *obj_p;

    size = RAT_ALIGN(size ? size : 1);

    if (p->chunks)
    {
        chunk *restrict chunk_p = p->chunks;
        if (chunk_p->cap - chunk_p->size >= size)
        {
            obj_p = chunk_p->data + chunk_p->size;
            chunk_p->size += size;

            goto end;
        }
    }

    size_t cap;
    if (p->chunkCap > size)
    {
        cap = ralcArena_chunkAllocSize(p->chunkCap);
    }
    else
    {
        assert(size <= (SIZE_MAX - HEADER_SIZE) / 2);
        cap = ralcArena_chunkAllocSize(size * 2);
    }

#ifndef NDEBUG
    size_t requestedCap = cap;
#endif
    unsigned char *restrict block_p = ralc_alloc(p->parentRalc, cap, &cap);
    if (!block_p) return NULL;
    assert(cap >= requestedCap);

    chunk *chunk_p = BLOCK_CHUNK(block_p);
    obj_p = chunk_p->data;
    chunk_p->next = p->chunks;
    chunk_p->cap = cap - HEADER_SIZE;
    chunk_p->size = size;

    p->chunks = chunk_p;

end:
    p->size += size;
    *out_actualSize = size;
    return obj_p;
}

ralcArena_mark_t ralcArena_mark(ralcArena *p)
{
    assert(p && p->parentRalc);

    return p->size;
}

void ralcArena_rewind(ralcArena *p, ralcArena_mark_t mark)
{
    assert(p && p->parentRalc && mark <= p->size);

    size_t size = p->size - mark;

    // Marks are trusted to come from this arena and not exceed p->size. Rewind
    // privately pops that byte distance, freeing whole chunks when crossed.
    while (size)
    {
        assert(p->chunks);

        chunk *restrict chunk_p = p->chunks;
        if (chunk_p->size > size)
        {
            chunk_p->size -= size;
            break;
        }

        size -= chunk_p->size;
        p->chunks = chunk_p->next;
        ralc_free(p->parentRalc, CHUNK_BLOCK(chunk_p));
    }

    p->size = mark;
}

size_t ralcArena_chunkAllocSize(size_t chunkCap)
{
    assert(chunkCap <= SIZE_MAX - HEADER_SIZE);
    return HEADER_SIZE + chunkCap;
}

void ralcArena_clear(ralcArena *p)
{
    assert(p && p->parentRalc);

    chunk *next_p;

    for (chunk *chunk_p = p->chunks; chunk_p; chunk_p = next_p)
    {
        next_p = chunk_p->next;
        ralc_free(p->parentRalc, CHUNK_BLOCK(chunk_p));
    }

    p->chunks = NULL;
    p->size = 0;
}

void ralcArena_getUsage(ralcArena *p, ralc_usage *out_usage)
{
    assert(p && p->parentRalc && out_usage);

    ralc_getUsage(p->parentRalc, out_usage);

    size_t holeSize = 0;

    size_t chunkCount = 0;
    size_t chunkHoleCount = 0;
    for (chunk *chunk_p = p->chunks; chunk_p; chunk_p = chunk_p->next)
    {
        ++chunkCount;
        assert(chunk_p->size <= chunk_p->cap);

        size_t remaining = chunk_p->cap - chunk_p->size;
        chunkHoleCount += remaining > 0;
        holeSize += remaining;
    }

    assert(chunkCount <= SIZE_MAX / HEADER_SIZE);
    size_t systemSize = chunkCount * HEADER_SIZE;
    assert(systemSize <= SIZE_MAX - holeSize && out_usage->blockSize >= systemSize + holeSize);

    out_usage->blockSize -= systemSize + holeSize;
    out_usage->holeCount += chunkHoleCount;
    out_usage->holeSize += holeSize;
    out_usage->systemSize += systemSize;
}
