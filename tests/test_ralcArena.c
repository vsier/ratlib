// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <rat/extra.h>
#include <rat/ralcArena.h>
#include <rat/ralcGen.h>
#include <rat/ralcPool.h>


static union
{
    size_t align;
    void *ptr;
    unsigned char bytes[4096];
} buf;

static int test_ralc_arena_zero_size_push_reserves_slot(void)
{
    ralcGen parent;
    ralcArena arena;
    ralc_usage usage;
    size_t actualSize1;
    size_t actualSize2;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcArena_init(&arena, 64, &parent.ifaceRalc);

    void *ptr1 = ralcArena_push(&arena, 0, &actualSize1);
    void *ptr2 = ralcArena_push(&arena, 0, &actualSize2);

    TEST_CHECK(ptr1 != NULL);
    TEST_CHECK(ptr2 != NULL);
    TEST_CHECK(ptr1 != ptr2);
    TEST_CHECK(actualSize1 == RAT_ALIGN(1));
    TEST_CHECK(actualSize2 == RAT_ALIGN(1));

    ralcArena_clear(&arena);
    TEST_CHECK(arena.size == 0);

    ralcArena_getUsage(&arena, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    ralcArena_clear(&arena);
    return 0;
}

static int test_ralc_arena_rewind_restores_marked_state(void)
{
    ralcGen parent;
    ralcArena arena;
    ralc_usage usageAtMark;
    ralc_usage usageAfterRewind;
    size_t actualSize;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcArena_init(&arena, 32, &parent.ifaceRalc);

    int *base = ralcArena_push(&arena, sizeof(int), &actualSize);
    TEST_CHECK(base != NULL);
    TEST_CHECK(arena.size == actualSize);
    *base = 123;

    ralcArena_mark_t mark = ralcArena_mark(&arena);
    TEST_CHECK(sizeof(mark) == sizeof(size_t));
    TEST_CHECK(mark == arena.size);
    ralcArena_getUsage(&arena, &usageAtMark);

    int *temp = ralcArena_push(&arena, sizeof(int), &actualSize);
    TEST_CHECK(temp != NULL);
    TEST_CHECK(temp != base);
    *temp = 456;

    TEST_CHECK(ralcArena_push(&arena, 128, &actualSize) != NULL);

    ralcArena_rewind(&arena, mark);
    TEST_CHECK(arena.size == mark);
    TEST_CHECK(*base == 123);

    ralcArena_getUsage(&arena, &usageAfterRewind);
    TEST_CHECK(usageAfterRewind.blockCount == usageAtMark.blockCount);
    TEST_CHECK(usageAfterRewind.blockSize == usageAtMark.blockSize);
    TEST_CHECK(usageAfterRewind.holeCount == usageAtMark.holeCount);
    TEST_CHECK(usageAfterRewind.holeSize == usageAtMark.holeSize);
    TEST_CHECK(usageAfterRewind.systemSize == usageAtMark.systemSize);

    TEST_CHECK(ralcArena_push(&arena, sizeof(int), &actualSize) == temp);

    ralcArena_clear(&arena);
    return 0;
}

static int test_ralc_arena_does_not_retry_smaller_chunk(void)
{
    ralcGen parent;
    ralcArena arena;
    size_t actualSize;

    union
    {
        size_t align;
        void *ptr;
        unsigned char bytes[64];
    } smallBuf;

    ralcGen_init(&parent, smallBuf.bytes, sizeof(smallBuf.bytes));
    ralcArena_init(&arena, 128, &parent.ifaceRalc);

    TEST_CHECK(ralcArena_push(&arena, sizeof(int), &actualSize) == NULL);
    TEST_CHECK(arena.chunks == NULL);
    TEST_CHECK(arena.size == 0);

    return 0;
}

static int test_ralc_arena_chunk_alloc_size_fits_pool_slot(void)
{
    ralcGen parent;
    ralcPool pool;
    ralcArena arena;
    size_t actualSize;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, ralcArena_chunkAllocSize(64), 1, &parent.ifaceRalc);
    ralcArena_init(&arena, 64, &pool.ifaceRalc);

    TEST_CHECK(ralcArena_push(&arena, sizeof(int), &actualSize) != NULL);
    TEST_CHECK(pool.count == 1);

    ralcArena_clear(&arena);
    TEST_CHECK(pool.count == 0);

    ralcPool_clear(&pool);
    return 0;
}

int test_ralcArena(void)
{
    TEST_RUN(test_ralc_arena_zero_size_push_reserves_slot);
    TEST_RUN(test_ralc_arena_rewind_restores_marked_state);
    TEST_RUN(test_ralc_arena_does_not_retry_smaller_chunk);
    TEST_RUN(test_ralc_arena_chunk_alloc_size_fits_pool_slot);

    return 0;
}
