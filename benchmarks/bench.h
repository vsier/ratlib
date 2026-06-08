// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_BENCH_H_
#define RAT_BENCH_H_

#include <rat/common.h>

#include <stdio.h>


#ifndef RAT_BENCH_BUF_SIZE
    #define RAT_BENCH_BUF_SIZE (16u * 1024u * 1024u)
#endif
#ifndef RAT_BENCH_MIN_SECONDS
    #define RAT_BENCH_MIN_SECONDS 0.10
#endif

typedef size_t bench_fn(void);

typedef union
{
    void *ptr;
    void (*fn)(void);
    long double ld;
    uintmax_t umax;
    unsigned char bytes[RAT_BENCH_BUF_SIZE];
} bench_buf;

extern size_t bench_count;
extern size_t bench_repeat;
extern bench_buf bench_allocBuf;
extern void **bench_ptrs;
extern size_t *bench_ringBuf;

void bench_fail(const char *file, int line, const char *expr);
size_t bench_opCount(size_t multiplier);
void bench_run(const char *name, size_t opCount, bench_fn *fn);

#define BENCH_REQUIRE(_expr) do { if (!(_expr)) bench_fail(__FILE__, __LINE__, #_expr); } while (0)

void bench_ralcGen(void);
void bench_ralcArena(void);
void bench_ralcPool(void);
void bench_rdyn(void);
void bench_rlist(void);
void bench_rmap(void);
void bench_rring(void);

#endif
