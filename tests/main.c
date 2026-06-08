// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <stdio.h>


int test_fail(const char *file, int line, const char *expr)
{
    fprintf(stderr, "%s:%d: check failed: %s\n", file, line, expr);
    return 1;
}

int test_run(const char *name, test_fn *fn)
{
    int result = fn();
    if (result)
        fprintf(stderr, "test failed: %s\n", name);

    return result;
}

int main(void)
{
    TEST_RUN(test_rhash);
    TEST_RUN(test_ralcArena);
    TEST_RUN(test_ralcPool);
    TEST_RUN(test_rdyn);
    TEST_RUN(test_rmap);
    TEST_RUN(test_rlist);
    TEST_RUN(test_rbit);
    TEST_RUN(test_rcrc);
    TEST_RUN(test_rcmp);
    TEST_RUN(test_rarr);
    TEST_RUN(test_rring);
    TEST_RUN(test_rorder);

    return 0;
}
