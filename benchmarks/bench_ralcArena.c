// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "bench.h"

#include <rat/ralcArena.h>
#include <rat/ralcGen.h>


static size_t bench_ralcArena_push_rewind(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        ralcGen parent;
        ralcArena arena;
        ralcGen_init(&parent, bench_allocBuf.bytes, sizeof(bench_allocBuf.bytes));
        ralcArena_init(&arena, 4096, &parent.ifaceRalc);

        ralcArena_mark_t mark = ralcArena_mark(&arena);
        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t actualSize;
            size_t *value_p = ralcArena_push(&arena, sizeof(*value_p), &actualSize);
            BENCH_REQUIRE(value_p);

            *value_p = i;
            sum += *value_p + actualSize;
        }
        ralcArena_rewind(&arena, mark);
        ralcArena_clear(&arena);
    }

    return sum;
}

void bench_ralcArena(void)
{
    bench_run("ralcArena push/rewind", bench_opCount(1), bench_ralcArena_push_rewind);
}
