// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "bench.h"

#include <rat/ralcGen.h>
#include <rat/rdyn.h>


static size_t bench_rdyn_push_pop(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        ralcGen parent;
        rdyn arr;
        ralcGen_init(&parent, bench_allocBuf.bytes, sizeof(bench_allocBuf.bytes));
        rdyn_init(&arr, sizeof(size_t), &parent.ifaceRalc);
        BENCH_REQUIRE(rdyn_setCap(&arr, bench_count));

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t *value_p = rdyn_push(&arr, 1);
            BENCH_REQUIRE(value_p);

            *value_p = i;
            sum += *value_p;
        }

        for (size_t i = 0; i < bench_count; ++i)
        {
            sum += arr.count;
            rdyn_pop(&arr, 1);
        }

        rdyn_clear(&arr);
    }

    return sum;
}

void bench_rdyn(void)
{
    bench_run("rdyn push/pop", bench_opCount(2), bench_rdyn_push_pop);
}
