// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "bench.h"

#include <rat/ralcGen.h>
#include <rat/rlist.h>


static size_t bench_rlist_push_remove(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        ralcGen parent;
        rlist list;
        ralcGen_init(&parent, bench_allocBuf.bytes, sizeof(bench_allocBuf.bytes));
        rlist_init(&list, &parent.ifaceRalc);

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t *value_p = rlist_push(&list, sizeof(*value_p));
            BENCH_REQUIRE(value_p);

            *value_p = i;
            sum += *value_p;
        }

        while (list.count)
        {
            size_t *value_p = list.first;
            sum += *value_p;
            rlist_remove(&list, value_p, 1);
        }
    }

    return sum;
}

void bench_rlist(void)
{
    bench_run("rlist push/remove", bench_opCount(2), bench_rlist_push_remove);
}
