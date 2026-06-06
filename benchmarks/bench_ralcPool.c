// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "bench.h"

#include <rat/ralcGen.h>
#include <rat/ralcPool.h>


static size_t bench_ralcPool_alloc_free(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        ralcGen parent;
        ralcPool pool;
        ralcGen_init(&parent, bench_allocBuf.bytes, sizeof(bench_allocBuf.bytes));
        ralcPool_init(&pool, sizeof(size_t), bench_count, &parent.ifaceRalc);

        for (size_t i = 0; i < bench_count; ++i)
        {
            bench_ptrs[i] = ralcPool_alloc(&pool);
            BENCH_REQUIRE(bench_ptrs[i]);

            *(size_t *)bench_ptrs[i] = i;
            sum += *(size_t *)bench_ptrs[i];
        }

        for (size_t i = bench_count; i > 0; --i)
        {
            sum += *(size_t *)bench_ptrs[i - 1];
            ralcPool_free(&pool, bench_ptrs[i - 1]);
        }

        ralcPool_clear(&pool);
    }

    return sum;
}

void bench_ralcPool(void)
{
    bench_run("ralcPool alloc/free", bench_opCount(2), bench_ralcPool_alloc_free);
}
