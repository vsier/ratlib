// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "bench.h"

#include <rat/rring.h>


static size_t bench_rring_push_pop(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        rring ring;
        rring_init(&ring, bench_ringBuf, sizeof(bench_ringBuf[0]), bench_count);

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t *value_p = rring_push(&ring);
            BENCH_REQUIRE(value_p);

            *value_p = i;
            sum += *value_p;
        }

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t *value_p = rring_last(&ring, 0);
            sum += *value_p;
            rring_pop(&ring, 1);
        }
    }

    return sum;
}

void bench_rring(void)
{
    bench_run("rring push/pop", bench_opCount(2), bench_rring_push_pop);
}
