// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "test.h"

#include <rat/extra.h>
#include <rat/ralcCustom.h>
#include <rat/ralcGen.h>
#include <rat/rdyn.h>
#include <string.h>


static union
{
    size_t align;
    void *ptr;
    unsigned char bytes[4096];
} buf;

typedef struct
{
    union
    {
        size_t align;
        void *ptr;
        unsigned char bytes[1024];
    } storage;
    size_t used;
    size_t allocCount;
    size_t reallocCount;
    size_t freeCount;
} allocOnlyRalc;

static void allocOnlyRalc_init(allocOnlyRalc *p)
{
    p->used = 0;
    p->allocCount = 0;
    p->reallocCount = 0;
    p->freeCount = 0;
}

static void *allocOnlyRalc_alloc(ralcCustom *p, size_t size, size_t *out_actualSize)
{
    allocOnlyRalc *state_p = p->userData;

    size = RAT_ALIGN(size);
    if (sizeof(state_p->storage.bytes) - state_p->used < size)
        return NULL;

    void *result = state_p->storage.bytes + state_p->used;
    state_p->used += size;
    ++state_p->allocCount;
    *out_actualSize = size;
    return result;
}

static void *allocOnlyRalc_realloc(ralcCustom *p, void *ptr, size_t newSize, size_t *out_actualNewSize)
{
    (void)ptr;
    (void)newSize;

    allocOnlyRalc *state_p = p->userData;
    ++state_p->reallocCount;
    *out_actualNewSize = 0;
    return NULL;
}

static void allocOnlyRalc_free(ralcCustom *p, void *ptr)
{
    allocOnlyRalc *state_p = p->userData;
    state_p->freeCount += ptr != NULL;
}

static void allocOnlyRalc_clear(ralcCustom *p)
{
    allocOnlyRalc_init(p->userData);
}

static void allocOnlyRalc_getUsage(ralcCustom *p, ralc_usage *out_usage)
{
    (void)p;

    out_usage->blockCount = 0;
    out_usage->blockSize = 0;
    out_usage->holeCount = 0;
    out_usage->holeSize = 0;
    out_usage->systemSize = 0;
}

static int test_rdyn_remove_valid_range_shifts_tail(void)
{
    ralcGen parent;
    rdyn dyn;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rdyn_init(&dyn, sizeof(int), &parent.ifaceRalc);

    int *values = rdyn_push(&dyn, 5);
    TEST_CHECK(values != NULL);
    for (int i = 0; i < 5; ++i)
        values[i] = i + 1;

    int *afterRemoved_p = rdyn_remove(&dyn, 1, 2);
    TEST_CHECK(afterRemoved_p == rdyn_at_t(&dyn, int, 1));
    TEST_CHECK(dyn.count == 3);
    TEST_CHECK(*rdyn_at_t(&dyn, int, 0) == 1);
    TEST_CHECK(*rdyn_at_t(&dyn, int, 1) == 4);
    TEST_CHECK(*rdyn_at_t(&dyn, int, 2) == 5);

    rdyn_clear(&dyn);

    return 0;
}

static int test_rdyn_grow_falls_back_to_alloc_when_realloc_fails(void)
{
    allocOnlyRalc state;
    ralcCustom parent;
    rdyn dyn;

    allocOnlyRalc_init(&state);
    ralcCustom_init(&parent, &state, allocOnlyRalc_alloc, allocOnlyRalc_realloc, allocOnlyRalc_free, allocOnlyRalc_clear, allocOnlyRalc_getUsage);
    rdyn_init(&dyn, sizeof(int), &parent.ifaceRalc);

    int *first = rdyn_push(&dyn, 2);
    TEST_CHECK(first != NULL);
    TEST_CHECK(state.reallocCount == 1);
    TEST_CHECK(state.allocCount == 1);

    first[0] = 11;
    first[1] = 22;

    void *oldBuf = dyn.buf;
    int *second = rdyn_push(&dyn, 4);
    TEST_CHECK(second != NULL);
    TEST_CHECK(dyn.buf != oldBuf);
    TEST_CHECK(state.reallocCount == 2);
    TEST_CHECK(state.allocCount == 2);
    TEST_CHECK(state.freeCount == 1);
    TEST_CHECK(dyn.count == 6);
    TEST_CHECK(*rdyn_at_t(&dyn, int, 0) == 11);
    TEST_CHECK(*rdyn_at_t(&dyn, int, 1) == 22);

    rdyn_clear(&dyn);
    TEST_CHECK(state.freeCount == 2);

    return 0;
}

static int test_rdyn_set_cap_zero_clears_array(void)
{
    ralcGen parent;
    rdyn dyn;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rdyn_init(&dyn, sizeof(int), &parent.ifaceRalc);

    int *values = rdyn_push(&dyn, 3);
    TEST_CHECK(values != NULL);
    TEST_CHECK(dyn.buf != NULL);
    TEST_CHECK(dyn.count == 3);
    TEST_CHECK(dyn.cap >= 3);

    TEST_CHECK(rdyn_setCap(&dyn, 0));
    TEST_CHECK(dyn.buf == NULL);
    TEST_CHECK(dyn.count == 0);
    TEST_CHECK(dyn.cap == 0);

    return 0;
}

static int test_rdyn_bin_search_asc(void)
{
    ralcGen parent;
    rdyn dyn;
    int arr[] = { 1, 2, 3, 3, 3, 5 };
    int target;
    size_t index;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rdyn_init(&dyn, sizeof(int), &parent.ifaceRalc);

    memcpy(rdyn_push(&dyn, RAT_LEN(arr)), arr, sizeof(arr));

    target = 2;
    TEST_CHECK(rdyn_binSearch(&dyn, rcmp_int32, &target, &index));
    TEST_CHECK(index == 1);
    target = 3;
    TEST_CHECK(rdyn_binSearch(&dyn, rcmp_int32, &target, &index));
    TEST_CHECK(index == 2);
    target = 4;
    TEST_CHECK(!rdyn_binSearch(&dyn, rcmp_int32, &target, &index));
    TEST_CHECK(index == 5);

    return 0;
}

static int test_rdyn_bin_search_desc(void)
{
    ralcGen parent;
    rdyn dyn;
    int arr[] = { 9, 7, 5, 5, 5, 3, 1 };
    int target;
    size_t index;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rdyn_init(&dyn, sizeof(int), &parent.ifaceRalc);

    memcpy(rdyn_push(&dyn, RAT_LEN(arr)), arr, sizeof(arr));

    target = 7;
    TEST_CHECK(rdyn_binSearch_desc(&dyn, rcmp_int32, &target, &index));
    TEST_CHECK(index == 1);
    target = 5;
    TEST_CHECK(rdyn_binSearch_desc(&dyn, rcmp_int32, &target, &index));
    TEST_CHECK(index == 4);
    target = 4;
    TEST_CHECK(!rdyn_binSearch_desc(&dyn, rcmp_int32, &target, &index));
    TEST_CHECK(index == 5);

    return 0;
}

int test_rdyn(void)
{
    TEST_RUN(test_rdyn_remove_valid_range_shifts_tail);
    TEST_RUN(test_rdyn_grow_falls_back_to_alloc_when_realloc_fails);
    TEST_RUN(test_rdyn_set_cap_zero_clears_array);
    TEST_RUN(test_rdyn_bin_search_asc);
    TEST_RUN(test_rdyn_bin_search_desc);

    return 0;
}
