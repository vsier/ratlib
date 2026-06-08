// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "test.h"

#include <rat/rbit.h>


static int test_rbit_size_conversions_round_up(void)
{
    TEST_CHECK(RBIT_BITSIZE(0) == 0);
    TEST_CHECK(RBIT_BITSIZE(1) == CHAR_BIT);
    TEST_CHECK(RBIT_BITSIZE(3) == 3 * CHAR_BIT);

    TEST_CHECK(RBIT_BYTESIZE(0) == 0);
    TEST_CHECK(RBIT_BYTESIZE(1) == 1);
    TEST_CHECK(RBIT_BYTESIZE(CHAR_BIT) == 1);
    TEST_CHECK(RBIT_BYTESIZE(CHAR_BIT + 1) == 2);

    return 0;
}

static int test_rbit_single_flag_macros(void)
{
    uintmax_t x = RBIT_FLAG_SET(UINTMAX_C(0), 3);

    TEST_CHECK(x == UINTMAX_C(8));
    TEST_CHECK(RBIT_FLAG_GET(x, 3));
    TEST_CHECK(!RBIT_FLAG_GET(x, 2));

    x = RBIT_FLAG_WRITE(x, 3, 0);
    TEST_CHECK(x == 0);

    x = RBIT_FLAG_WRITE(x, 5, 2);
    TEST_CHECK(x == RBIT(5));

    return 0;
}

static int test_rbit_flag_set_queries(void)
{
    uintmax_t flags = RBIT_FLAGS(RBIT(1) | RBIT(3));
    uintmax_t x = RBIT_FLAGS_SET(RBIT(0), flags);

    TEST_CHECK(x == (RBIT(0) | RBIT(1) | RBIT(3)));
    TEST_CHECK(RBIT_FLAGS_GET(x, flags) == flags);
    TEST_CHECK(RBIT_FLAGS_ALL(x, flags));
    TEST_CHECK(RBIT_FLAGS_ANY(x, RBIT(3) | RBIT(4)));
    TEST_CHECK(!RBIT_FLAGS_ANY(x, RBIT(4)));
    TEST_CHECK(RBIT_FLAGS_ONLY(flags, x));
    TEST_CHECK(!RBIT_FLAGS_ONLY(x, flags));
    TEST_CHECK(RBIT_FLAGS_EQUAL(flags, RBIT(1) | RBIT(3)));

    x = RBIT_FLAGS_UNSET(x, RBIT(1) | RBIT(4));
    TEST_CHECK(x == (RBIT(0) | RBIT(3)));

    return 0;
}

static int test_rbit_masks_and_fields(void)
{
    TEST_CHECK(RBIT_MASK(4, 3) == (RBIT(4) | RBIT(5) | RBIT(6)));
    TEST_CHECK(RBIT_MASK_FULL == UINTMAX_MAX);

    uintmax_t x = RBIT_FIELD(4, 3, UINTMAX_C(5));
    TEST_CHECK(x == (RBIT(4) | RBIT(6)));
    TEST_CHECK(RBIT_FIELD_GET(x, 4, 3) == 5);

    x = RBIT_FIELD_SET(x, 8, 2);
    TEST_CHECK(RBIT_FIELD_GET(x, 8, 2) == 3);

    x = RBIT_FIELD_UNSET(x, 5, 4);
    TEST_CHECK(x == (RBIT(4) | RBIT(9)));

    x = RBIT_FIELD_WRITE(x, 4, 3, 2);
    TEST_CHECK(RBIT_FIELD_GET(x, 4, 3) == 2);
    TEST_CHECK((x & ~RBIT_MASK(4, 3)) == RBIT(9));

    return 0;
}

int test_rbit(void)
{
    TEST_RUN(test_rbit_size_conversions_round_up);
    TEST_RUN(test_rbit_single_flag_macros);
    TEST_RUN(test_rbit_flag_set_queries);
    TEST_RUN(test_rbit_masks_and_fields);

    return 0;
}
