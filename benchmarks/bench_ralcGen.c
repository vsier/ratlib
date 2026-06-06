// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "bench.h"

#include <rat/ralcGen.h>


static size_t bench_ralcGen_alloc_free(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        ralcGen parent;
        ralcGen_init(&parent, bench_allocBuf.bytes, sizeof(bench_allocBuf.bytes));

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t actualSize;
            bench_ptrs[i] = ralcGen_alloc(&parent, sizeof(size_t), &actualSize);
            BENCH_REQUIRE(bench_ptrs[i]);

            *(size_t *)bench_ptrs[i] = i;
            sum += actualSize;
        }

        for (size_t i = bench_count; i > 0; --i)
        {
            size_t *value_p = bench_ptrs[i - 1];
            sum += *value_p;
            ralcGen_free(&parent, value_p);
        }
    }

    return sum;
}

void bench_ralcGen(void)
{
    bench_run("ralcGen alloc/free", bench_opCount(2), bench_ralcGen_alloc_free);
}
