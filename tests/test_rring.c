// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "test.h"

#include <rat/rring.h>


static int test_rring_push_pop_and_index_from_back(void)
{
    int buf[4];
    rring ring;

    rring_init(&ring, buf, sizeof(buf[0]), 4);

    int *a = rring_push(&ring);
    int *b = rring_push(&ring);
    int *c = rring_push(&ring);

    TEST_CHECK(a == &buf[0]);
    TEST_CHECK(b == &buf[1]);
    TEST_CHECK(c == &buf[2]);
    TEST_CHECK(ring.count == 3);

    *a = 11;
    *b = 22;
    *c = 33;

    TEST_CHECK(*(int *)rring_at(&ring, 0) == 11);
    TEST_CHECK(*(int *)rring_at(&ring, 1) == 22);
    TEST_CHECK(*(int *)rring_at(&ring, 2) == 33);
    TEST_CHECK(*(int *)rring_last(&ring, 0) == 33);
    TEST_CHECK(*(int *)rring_last(&ring, 1) == 22);

    rring_pop(&ring, 2);
    TEST_CHECK(ring.count == 1);
    TEST_CHECK(*(int *)rring_at(&ring, 0) == 11);

    return 0;
}

static int test_rring_push_front_pop_front_and_clear(void)
{
    int buf[4];
    rring ring;

    rring_init(&ring, buf, sizeof(buf[0]), 4);

    *(int *)rring_push(&ring) = 22;
    *(int *)rring_pushFront(&ring) = 11;
    *(int *)rring_push(&ring) = 33;

    TEST_CHECK(ring.count == 3);
    TEST_CHECK(*(int *)rring_at(&ring, 0) == 11);
    TEST_CHECK(*(int *)rring_at(&ring, 1) == 22);
    TEST_CHECK(*(int *)rring_at(&ring, 2) == 33);

    rring_popFront(&ring, 2);
    TEST_CHECK(ring.count == 1);
    TEST_CHECK(*(int *)rring_at(&ring, 0) == 33);

    rring_clear(&ring);
    TEST_CHECK(ring.first == 0);
    TEST_CHECK(ring.count == 0);

    return 0;
}

static int test_rring_wraps_storage_indices(void)
{
    int buf[3];
    rring ring;

    rring_init(&ring, buf, sizeof(buf[0]), 3);

    *(int *)rring_push(&ring) = 1;
    *(int *)rring_push(&ring) = 2;
    *(int *)rring_push(&ring) = 3;
    rring_popFront(&ring, 2);

    int *a = rring_push(&ring);
    int *b = rring_push(&ring);

    TEST_CHECK(a == &buf[0]);
    TEST_CHECK(b == &buf[1]);
    *a = 4;
    *b = 5;

    TEST_CHECK(ring.count == 3);
    TEST_CHECK(*(int *)rring_at(&ring, 0) == 3);
    TEST_CHECK(*(int *)rring_at(&ring, 1) == 4);
    TEST_CHECK(*(int *)rring_at(&ring, 2) == 5);
    TEST_CHECK(*(int *)rring_last(&ring, 0) == 5);

    return 0;
}

static int test_rring_returns_null_when_full(void)
{
    int buf[2];
    rring ring;

    rring_init(&ring, buf, sizeof(buf[0]), 2);

    TEST_CHECK(rring_push(&ring) != NULL);
    TEST_CHECK(rring_push(&ring) != NULL);
    TEST_CHECK(rring_push(&ring) == NULL);
    TEST_CHECK(rring_pushFront(&ring) == NULL);
    TEST_CHECK(ring.count == 2);

    return 0;
}

int test_rring(void)
{
    TEST_RUN(test_rring_push_pop_and_index_from_back);
    TEST_RUN(test_rring_push_front_pop_front_and_clear);
    TEST_RUN(test_rring_wraps_storage_indices);
    TEST_RUN(test_rring_returns_null_when_full);

    return 0;
}
