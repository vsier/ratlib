// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "bench.h"

#include <rat/ralcGen.h>
#include <rat/rmap.h>


static size_t bench_rmap_set_get_remove(void)
{
    size_t sum = 0;

    for (size_t repeat = 0; repeat < bench_repeat; ++repeat)
    {
        ralcGen parent;
        rmap map;
        ralcGen_init(&parent, bench_allocBuf.bytes, sizeof(bench_allocBuf.bytes));
        rmap_init(&map, UINT32_C(0x9e3779b9), UINT32_C(0x7f4a7c15), NULL, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t *value_p = rmap_set(&map, &i, sizeof(i), sizeof(*value_p));
            BENCH_REQUIRE(value_p);

            *value_p = i;
            sum += *value_p;
        }

        for (size_t i = 0; i < bench_count; ++i)
        {
            size_t *value_p = rmap_get(&map, &i, sizeof(i));
            BENCH_REQUIRE(value_p);
            sum += *value_p;
        }

        for (size_t i = 0; i < bench_count; ++i)
        {
            sum += rmap_remove(&map, &i, sizeof(i));
        }

        rmap_clear(&map);
    }

    return sum;
}

void bench_rmap(void)
{
    bench_run("rmap set/get/remove", bench_opCount(3), bench_rmap_set_get_remove);
}
