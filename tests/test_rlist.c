// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "test.h"

#include <rat/ralcGen.h>
#include <rat/ralcPool.h>
#include <rat/rlist.h>


static union
{
    size_t align;
    void *ptr;
    unsigned char bytes[8192];
} buf;

static int test_rlist_init_clear_and_empty_list(void)
{
    ralcGen parent;
    rlist list;
    ralc_usage usage;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    TEST_CHECK(list.first == NULL);
    TEST_CHECK(list.last == NULL);
    TEST_CHECK(list.count == 0);

    TEST_CHECK(rlist_remove(&list, NULL, 0) == NULL);
    TEST_CHECK(list.first == NULL);
    TEST_CHECK(list.last == NULL);
    TEST_CHECK(list.count == 0);

    rlist_getUsage(&list, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    rlist_clear(&list);
    TEST_CHECK(list.first == NULL);
    TEST_CHECK(list.last == NULL);
    TEST_CHECK(list.count == 0);

    ralcGen_getUsage(&parent, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    return 0;
}

static int test_rlist_push_insert_and_links(void)
{
    ralcGen parent;
    rlist list;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    int *a = rlist_push(&list, sizeof(*a));
    TEST_CHECK(a != NULL);
    *a = 1;

    TEST_CHECK(list.count == 1);
    TEST_CHECK(list.first == a);
    TEST_CHECK(list.last == a);
    TEST_CHECK(rlist_prev(a) == NULL);
    TEST_CHECK(rlist_next(a) == NULL);
    TEST_CHECK(rlist_elemSize(a) >= sizeof(*a));

    int *b = rlist_push(&list, sizeof(*b));
    TEST_CHECK(b != NULL);
    *b = 2;

    int *c = rlist_pushFront(&list, sizeof(*c));
    TEST_CHECK(c != NULL);
    *c = 3;

    int *d = rlist_insert(&list, b, sizeof(*d));
    TEST_CHECK(d != NULL);
    *d = 4;

    TEST_CHECK(list.count == 4);
    TEST_CHECK(list.first == c);
    TEST_CHECK(list.last == b);

    TEST_CHECK(rlist_at(&list, 0) == c);
    TEST_CHECK(rlist_at(&list, 1) == a);
    TEST_CHECK(rlist_at(&list, 2) == d);
    TEST_CHECK(rlist_at(&list, 3) == b);

    TEST_CHECK(rlist_index(&list, c) == 0);
    TEST_CHECK(rlist_index(&list, a) == 1);
    TEST_CHECK(rlist_index(&list, d) == 2);
    TEST_CHECK(rlist_index(&list, b) == 3);

    TEST_CHECK(rlist_prev(c) == NULL);
    TEST_CHECK(rlist_next(c) == a);
    TEST_CHECK(rlist_prev(a) == c);
    TEST_CHECK(rlist_next(a) == d);
    TEST_CHECK(rlist_prev(d) == a);
    TEST_CHECK(rlist_next(d) == b);
    TEST_CHECK(rlist_prev(b) == d);
    TEST_CHECK(rlist_next(b) == NULL);

    rlist_clear(&list);
    return 0;
}

static int test_rlist_push_front_in_empty_list(void)
{
    ralcGen parent;
    rlist list;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    int *a = rlist_pushFront(&list, sizeof(*a));
    TEST_CHECK(a != NULL);
    *a = 11;

    TEST_CHECK(list.count == 1);
    TEST_CHECK(list.first == a);
    TEST_CHECK(list.last == a);
    TEST_CHECK(rlist_prev(a) == NULL);
    TEST_CHECK(rlist_next(a) == NULL);

    rlist_clear(&list);
    return 0;
}

static int test_rlist_failed_first_insert_restores_empty_state(void)
{
    ralcGen parent;
    ralcPool pool;
    rlist list;
    ralc_usage usage;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, rlist_elemAllocSize(0), 1, &parent.ifaceRalc);
    rlist_init(&list, &pool.ifaceRalc);

    TEST_CHECK(rlist_push(&list, sizeof(int)) == NULL);
    TEST_CHECK(list.first == NULL);
    TEST_CHECK(list.last == NULL);
    TEST_CHECK(list.count == 0);
    TEST_CHECK(pool.count == 0);

    rlist_getUsage(&list, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    ralcPool_clear(&pool);
    return 0;
}

static int test_rlist_resize_preserves_links_and_payload(void)
{
    ralcGen parent;
    rlist list;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    int *a = rlist_push(&list, sizeof(*a));
    int *b = rlist_push(&list, sizeof(*b));
    TEST_CHECK(a != NULL);
    TEST_CHECK(b != NULL);
    *a = 11;
    *b = 22;

    int *newA = rlist_resize(&list, a, 2000);
    TEST_CHECK(newA != NULL);
    TEST_CHECK(rlist_elemSize(newA) >= 2000);
    TEST_CHECK(*(int *)newA == 11);
    TEST_CHECK(list.count == 2);
    TEST_CHECK(list.first == newA);
    TEST_CHECK(list.last == b);
    TEST_CHECK(rlist_prev(newA) == NULL);
    TEST_CHECK(rlist_next(newA) == b);
    TEST_CHECK(rlist_prev(b) == newA);
    TEST_CHECK(rlist_next(b) == NULL);

    int *newB = rlist_resize(&list, b, 3000);
    TEST_CHECK(newB != NULL);
    TEST_CHECK(rlist_elemSize(newB) >= 3000);
    TEST_CHECK(*(int *)newB == 22);
    TEST_CHECK(list.count == 2);
    TEST_CHECK(list.first == newA);
    TEST_CHECK(list.last == newB);
    TEST_CHECK(rlist_prev(newB) == newA);
    TEST_CHECK(rlist_next(newA) == newB);
    TEST_CHECK(rlist_next(newB) == NULL);

    rlist_clear(&list);
    return 0;
}

static int test_rlist_resize_falls_back_with_alloc_only_parent(void)
{
    ralcGen parent;
    ralcPool pool;
    rlist list;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, rlist_elemAllocSize(64), 2, &parent.ifaceRalc);
    rlist_init(&list, &pool.ifaceRalc);

    int *a = rlist_push(&list, sizeof(*a));
    int *b = rlist_push(&list, sizeof(*b));
    TEST_CHECK(a != NULL);
    TEST_CHECK(b != NULL);
    *a = 11;
    *b = 22;

    int *newA = rlist_resize(&list, a, 32);
    TEST_CHECK(newA != NULL);
    TEST_CHECK(*(int *)newA == 11);
    TEST_CHECK(list.count == 2);
    TEST_CHECK(list.first == newA);
    TEST_CHECK(list.last == b);
    TEST_CHECK(rlist_prev(newA) == NULL);
    TEST_CHECK(rlist_next(newA) == b);
    TEST_CHECK(rlist_prev(b) == newA);
    TEST_CHECK(rlist_next(b) == NULL);

    rlist_clear(&list);
    TEST_CHECK(pool.count == 0);

    ralcPool_clear(&pool);
    return 0;
}

static int test_rlist_remove_updates_edges(void)
{
    ralcGen parent;
    rlist list;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    int *a = rlist_push(&list, sizeof(*a));
    int *b = rlist_push(&list, sizeof(*b));
    int *c = rlist_push(&list, sizeof(*c));
    int *d = rlist_push(&list, sizeof(*d));
    TEST_CHECK(a != NULL);
    TEST_CHECK(b != NULL);
    TEST_CHECK(c != NULL);
    TEST_CHECK(d != NULL);

    TEST_CHECK(rlist_remove(&list, NULL, 0) == NULL);
    TEST_CHECK(list.count == 4);
    TEST_CHECK(list.first == a);
    TEST_CHECK(list.last == d);

    TEST_CHECK(rlist_remove(&list, b, 2) == d);
    TEST_CHECK(list.count == 2);
    TEST_CHECK(list.first == a);
    TEST_CHECK(list.last == d);
    TEST_CHECK(rlist_prev(a) == NULL);
    TEST_CHECK(rlist_next(a) == d);
    TEST_CHECK(rlist_prev(d) == a);
    TEST_CHECK(rlist_next(d) == NULL);

    TEST_CHECK(rlist_remove(&list, a, 1) == d);
    TEST_CHECK(list.count == 1);
    TEST_CHECK(list.first == d);
    TEST_CHECK(list.last == d);
    TEST_CHECK(rlist_prev(d) == NULL);
    TEST_CHECK(rlist_next(d) == NULL);

    TEST_CHECK(rlist_remove(&list, d, 1) == NULL);
    TEST_CHECK(list.first == NULL);
    TEST_CHECK(list.last == NULL);
    TEST_CHECK(list.count == 0);

    rlist_clear(&list);
    return 0;
}

static int test_rlist_clear_releases_backing_allocations(void)
{
    ralcGen parent;
    rlist list;
    ralc_usage usage;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    TEST_CHECK(rlist_push(&list, sizeof(int)) != NULL);
    TEST_CHECK(rlist_push(&list, sizeof(int)) != NULL);
    TEST_CHECK(rlist_push(&list, sizeof(int)) != NULL);

    rlist_getUsage(&list, &usage);
    TEST_CHECK(usage.blockCount == 3);
    TEST_CHECK(usage.blockSize >= 3 * sizeof(int));
    TEST_CHECK(usage.systemSize > 0);

    rlist_clear(&list);
    TEST_CHECK(list.first == NULL);
    TEST_CHECK(list.last == NULL);
    TEST_CHECK(list.count == 0);

    ralcGen_getUsage(&parent, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    return 0;
}

static int test_rlist_generic_allocator_interface(void)
{
    ralcGen parent;
    rlist list;
    size_t actualSize;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rlist_init(&list, &parent.ifaceRalc);

    int *a = ralc_alloc(&list.ifaceRalc, sizeof(*a), &actualSize);
    TEST_CHECK(a != NULL);
    TEST_CHECK(actualSize >= sizeof(*a));
    TEST_CHECK(list.count == 1);
    TEST_CHECK(list.first == a);
    TEST_CHECK(list.last == a);
    *a = 44;

    int *b = ralc_realloc(&list.ifaceRalc, NULL, sizeof(*b), &actualSize);
    TEST_CHECK(b != NULL);
    TEST_CHECK(actualSize >= sizeof(*b));
    TEST_CHECK(list.count == 2);
    TEST_CHECK(list.first == a);
    TEST_CHECK(list.last == b);
    *b = 55;

    int *newA = ralc_realloc(&list.ifaceRalc, a, 2048, &actualSize);
    TEST_CHECK(newA != NULL);
    TEST_CHECK(actualSize >= 2048);
    TEST_CHECK(*(int *)newA == 44);
    TEST_CHECK(list.count == 2);
    TEST_CHECK(list.first == newA);
    TEST_CHECK(list.last == b);
    TEST_CHECK(rlist_next(newA) == b);
    TEST_CHECK(rlist_prev(b) == newA);

    ralc_free(&list.ifaceRalc, newA);
    TEST_CHECK(list.count == 1);
    TEST_CHECK(list.first == b);
    TEST_CHECK(list.last == b);
    TEST_CHECK(rlist_prev(b) == NULL);
    TEST_CHECK(rlist_next(b) == NULL);

    rlist_clear(&list);
    return 0;
}

static int test_rlist_elem_alloc_size_fits_pool_slot(void)
{
    ralcGen parent;
    ralcPool pool;
    rlist list;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    ralcPool_init(&pool, rlist_elemAllocSize(sizeof(int)), 2, &parent.ifaceRalc);
    rlist_init(&list, &pool.ifaceRalc);

    int *a = rlist_push(&list, sizeof(*a));
    int *b = rlist_push(&list, sizeof(*b));
    TEST_CHECK(a != NULL);
    TEST_CHECK(b != NULL);
    TEST_CHECK(rlist_elemSize(b) >= sizeof(*b));
    TEST_CHECK(pool.count == 2);

    ralc_usage usage;
    rlist_getUsage(&list, &usage);
    TEST_CHECK(usage.blockCount == 2);
    TEST_CHECK(usage.blockSize == rlist_elemSize(a) + rlist_elemSize(b));

    rlist_clear(&list);
    TEST_CHECK(pool.count == 0);

    ralcPool_clear(&pool);
    return 0;
}

int test_rlist(void)
{
    TEST_RUN(test_rlist_init_clear_and_empty_list);
    TEST_RUN(test_rlist_push_insert_and_links);
    TEST_RUN(test_rlist_push_front_in_empty_list);
    TEST_RUN(test_rlist_failed_first_insert_restores_empty_state);
    TEST_RUN(test_rlist_resize_preserves_links_and_payload);
    TEST_RUN(test_rlist_resize_falls_back_with_alloc_only_parent);
    TEST_RUN(test_rlist_remove_updates_edges);
    TEST_RUN(test_rlist_clear_releases_backing_allocations);
    TEST_RUN(test_rlist_generic_allocator_interface);
    TEST_RUN(test_rlist_elem_alloc_size_fits_pool_slot);

    return 0;
}
