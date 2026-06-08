# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

file(GLOB RAT_TEST_SOURCES CONFIGURE_DEPENDS "test_*.c")

add_executable(rat_tests "main.c" ${RAT_TEST_SOURCES})

target_link_libraries(rat_tests PRIVATE rat)

add_test(NAME rat_tests COMMAND rat_tests)
