// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <rat/rcmp.h>


static int cmpSign(int cmp)
{
    return (cmp > 0) - (cmp < 0);
}

static int test_rcmp_str_compares_embedded_nul_bytes(void)
{
    const char left[] = { 'a', '\0', 'b' };
    const char right[] = { 'a', '\0', 'a' };

    TEST_CHECK(cmpSign(rcmp_str(left, sizeof(left), right, sizeof(right))) > 0);
    TEST_CHECK(cmpSign(rcmp_str(right, sizeof(right), left, sizeof(left))) < 0);

    return 0;
}

static int test_rcmp_str_compares_lexicographically(void)
{
    TEST_CHECK(cmpSign(rcmp_str("b", 1, "aa", 2)) > 0);
    TEST_CHECK(cmpSign(rcmp_str("a", 1, "aa", 2)) < 0);

    return 0;
}

static int test_rcmp_cstr_uses_nul_termination(void)
{
    const char left[] = { 'a', '\0', 'b', '\0' };
    const char right[] = { 'a', '\0', 'a', '\0' };

    TEST_CHECK(rcmp_cstr(left, sizeof(left), right, sizeof(right)) == 0);

    return 0;
}

int test_rcmp(void)
{
    TEST_RUN(test_rcmp_str_compares_embedded_nul_bytes);
    TEST_RUN(test_rcmp_str_compares_lexicographically);
    TEST_RUN(test_rcmp_cstr_uses_nul_termination);

    return 0;
}
