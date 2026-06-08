# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

file(GLOB RAT_BENCH_SOURCES CONFIGURE_DEPENDS "bench_*.c")

add_executable(rat_benchmarks "main.c" ${RAT_BENCH_SOURCES})

target_link_libraries(rat_benchmarks PRIVATE rat)
