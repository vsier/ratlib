// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <rat/ralcGen.h>
#include <rat/ralcPool.h>


static union
{
    size_t align;
    void *ptr;
    unsigned char bytes[4096];
} buf;

static int test_ralc_pool_alloc_free_and_reuse_slot(void)
{
    ralcGen parent;
    ralcPool pool;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, sizeof(int), 2, &parent.ifaceRalc);

    int *a = ralcPool_alloc(&pool);
    int *b = ralcPool_alloc(&pool);
    TEST_CHECK(a != NULL);
    TEST_CHECK(b != NULL);
    TEST_CHECK(a != b);

    *a = 11;
    *b = 22;

    ralcPool_free(&pool, a);
    int *c = ralcPool_alloc(&pool);
    TEST_CHECK(c == a);
    TEST_CHECK(*b == 22);

    ralcPool_clear(&pool);
    return 0;
}

static int test_ralc_pool_grows_without_moving_existing_slots(void)
{
    ralcGen parent;
    ralcPool pool;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, sizeof(int), 2, &parent.ifaceRalc);

    int *a = ralcPool_alloc(&pool);
    int *b = ralcPool_alloc(&pool);
    *a = 10;
    *b = 20;

    int *c = ralcPool_alloc(&pool);
    TEST_CHECK(c != NULL);
    TEST_CHECK(c != a);
    TEST_CHECK(c != b);
    TEST_CHECK(*a == 10);
    TEST_CHECK(*b == 20);

    ralcPool_clear(&pool);
    return 0;
}

static int test_ralc_pool_generic_interface_allocates_and_frees(void)
{
    ralcGen parent;
    ralcPool pool;
    ralc_usage usage;
    size_t actualSize;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, sizeof(int), 0, &parent.ifaceRalc);
    TEST_CHECK(pool.chunkCap == 1);

    int *a = ralc_alloc(&pool.ifaceRalc, sizeof(int), &actualSize);
    TEST_CHECK(a != NULL);
    TEST_CHECK(actualSize >= sizeof(int));
    *a = 123;

    ralc_getUsage(&pool.ifaceRalc, &usage);
    TEST_CHECK(usage.blockCount == 1);
    TEST_CHECK(usage.blockSize >= sizeof(int));

    ralc_free(&pool.ifaceRalc, a);
    ralc_getUsage(&pool.ifaceRalc, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    int *b = ralc_realloc(&pool.ifaceRalc, NULL, sizeof(int), &actualSize);
    TEST_CHECK(b != NULL);
    TEST_CHECK(ralc_realloc(&pool.ifaceRalc, b, sizeof(int), &actualSize) == NULL);
    ralc_free(&pool.ifaceRalc, b);

    ralcPool_clear(&pool);
    return 0;
}

static int test_ralc_pool_rejects_oversized_allocations(void)
{
    ralcGen parent;
    ralcPool pool;
    size_t actualSize = 0;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, sizeof(int), 2, &parent.ifaceRalc);

    TEST_CHECK(ralc_alloc(&pool.ifaceRalc, pool.elemSize + 1, &actualSize) == NULL);
    TEST_CHECK(pool.count == 0);

    ralcPool_clear(&pool);
    return 0;
}

static int test_ralc_pool_does_not_retry_single_slot_chunk(void)
{
    ralcGen parent;
    ralcPool pool;

    union
    {
        size_t align;
        void *ptr;
        unsigned char bytes[64];
    } smallBuf;

    ralcGen_init(&parent, smallBuf.bytes, sizeof(smallBuf.bytes));
    ralcPool_init(&pool, sizeof(int), 8, &parent.ifaceRalc);

    TEST_CHECK(ralcPool_alloc(&pool) == NULL);
    TEST_CHECK(pool.chunks == NULL);
    TEST_CHECK(pool.freeSlots == NULL);
    TEST_CHECK(pool.count == 0);

    return 0;
}

int test_ralcPool(void)
{
    TEST_RUN(test_ralc_pool_alloc_free_and_reuse_slot);
    TEST_RUN(test_ralc_pool_grows_without_moving_existing_slots);
    TEST_RUN(test_ralc_pool_generic_interface_allocates_and_frees);
    TEST_RUN(test_ralc_pool_rejects_oversized_allocations);
    TEST_RUN(test_ralc_pool_does_not_retry_single_slot_chunk);

    return 0;
}
