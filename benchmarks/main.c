// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "bench.h"

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#ifdef RAT_CONFIG_OS_Windows
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#else
#include <time.h>
#endif


static volatile size_t bench_sink;

size_t bench_count;
size_t bench_repeat;
bench_buf bench_allocBuf;
void **bench_ptrs;
size_t *bench_ringBuf;


void bench_fail(const char *file, int line, const char *expr)
{
    fprintf(stderr, "%s:%d: benchmark requirement failed: %s\n", file, line, expr);
    exit(1);
}

static void bench_usage(const char *exe)
{
    fprintf(stderr, "usage: %s count repeat\n", exe);
    exit(2);
}

static size_t bench_parseSize(const char *str, const char *exe)
{
    char *end;
    errno = 0;
    uintmax_t value = strtoumax(str, &end, 10);
    if (!str[0] || *end || errno == ERANGE || value == 0 || value > SIZE_MAX)
        bench_usage(exe);

    return (size_t)value;
}

static void bench_init(int argc, char **argv)
{
    if (argc != 3)
        bench_usage(argv[0]);

    bench_count = bench_parseSize(argv[1], argv[0]);
    bench_repeat = bench_parseSize(argv[2], argv[0]);

    BENCH_REQUIRE(bench_count <= SIZE_MAX / sizeof(*bench_ptrs));
    BENCH_REQUIRE(bench_count <= SIZE_MAX / sizeof(*bench_ringBuf));

    bench_ptrs = malloc(bench_count * sizeof(*bench_ptrs));
    bench_ringBuf = malloc(bench_count * sizeof(*bench_ringBuf));
    BENCH_REQUIRE(bench_ptrs && bench_ringBuf);
}

#ifdef RAT_CONFIG_OS_Windows
typedef struct
{
    LARGE_INTEGER counter;
} bench_time;

static bench_time bench_timeNow(void)
{
    bench_time result;
    BENCH_REQUIRE(QueryPerformanceCounter(&result.counter));
    return result;
}

static double bench_timeSeconds(bench_time begin, bench_time end)
{
    static double freq = 0.0;
    if (freq == 0.0)
    {
        LARGE_INTEGER freqCounter;
        BENCH_REQUIRE(QueryPerformanceFrequency(&freqCounter) && freqCounter.QuadPart > 0);
        freq = (double)freqCounter.QuadPart;
    }
    BENCH_REQUIRE(end.counter.QuadPart >= begin.counter.QuadPart);

    return (double)(end.counter.QuadPart - begin.counter.QuadPart) / freq;
}
#else
typedef struct
{
    clock_t clock;
    time_t time;
} bench_time;

static bench_time bench_timeNow(void)
{
    bench_time result;
    result.clock = clock();
    result.time = time(NULL);
    BENCH_REQUIRE(result.time != (time_t)-1);
    return result;
}

static double bench_timeSeconds(bench_time begin, bench_time end)
{
    if (begin.clock != (clock_t)-1 && end.clock != (clock_t)-1 && end.clock >= begin.clock)
        return (double)(end.clock - begin.clock) / (double)CLOCKS_PER_SEC;

    return difftime(end.time, begin.time);
}
#endif

void bench_run(const char *name, size_t opCount, bench_fn *fn)
{
    bench_time begin = bench_timeNow();
    bench_time end = begin;
    double seconds = 0.0;
    size_t result = 0;
    size_t rounds = 0;

    do
    {
        result += fn();
        ++rounds;
        end = bench_timeNow();
        seconds = bench_timeSeconds(begin, end);
    } while (seconds < RAT_BENCH_MIN_SECONDS);

    BENCH_REQUIRE(opCount <= SIZE_MAX / rounds);

    bench_sink += result;

    opCount *= rounds;
    double nsPerOp = seconds > 0.0 ? seconds * 1000000000.0 / (double)opCount : 0.0;

    printf("%-24s %12zu %12.6f %12.2f\n", name, opCount, seconds, nsPerOp);
}

size_t bench_opCount(size_t multiplier)
{
    BENCH_REQUIRE(bench_repeat <= SIZE_MAX / bench_count);
    size_t result = bench_repeat * bench_count;
    BENCH_REQUIRE(multiplier <= SIZE_MAX / result);
    return multiplier * result;
}

int main(int argc, char **argv)
{
    bench_init(argc, argv);

    printf("ratlib benchmarks: count=%zu repeat=%zu\n", bench_count, bench_repeat);
    printf("%-24s %12s %12s %12s\n", "benchmark", "ops", "seconds", "ns/op");

    bench_ralcGen();
    bench_ralcArena();
    bench_ralcPool();
    bench_rdyn();
    bench_rlist();
    bench_rmap();
    bench_rring();

    free(bench_ringBuf);
    free(bench_ptrs);

    return bench_sink == 0 ? 1 : 0;
}
