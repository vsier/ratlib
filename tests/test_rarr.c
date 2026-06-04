// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <rat/rarr.h>
#include <rat/rcmp.h>


static int test_rarr_bin_search_finds_existing_ascending_value(void)
{
    int arr[] = { 1, 3, 5, 7, 9 };
    int target = 5;
    size_t index = 999;

    TEST_CHECK(rarr_binSearch(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 2);

    return 0;
}

static int test_rarr_bin_search_returns_ascending_insertion_index(void)
{
    int arr[] = { 1, 3, 5, 7, 9 };
    int target = 6;
    size_t index = 999;

    TEST_CHECK(!rarr_binSearch(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 3);

    target = 0;
    TEST_CHECK(!rarr_binSearch(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 0);

    target = 10;
    TEST_CHECK(!rarr_binSearch(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == sizeof(arr) / sizeof(arr[0]));

    return 0;
}

static int test_rarr_bin_search_returns_leftmost_duplicate(void)
{
    int arr[] = { 1, 3, 3, 3, 5 };
    int target = 3;
    size_t index = 999;

    TEST_CHECK(rarr_binSearch(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 1);

    target = 4;
    TEST_CHECK(!rarr_binSearch(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 4);

    return 0;
}

static int test_rarr_bin_search_desc_finds_existing_descending_value(void)
{
    int arr[] = { 9, 7, 5, 3, 1 };
    int target = 5;
    size_t index = 999;

    TEST_CHECK(rarr_binSearch_desc(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 2);

    return 0;
}

static int test_rarr_bin_search_desc_returns_rightmost_duplicate(void)
{
    int arr[] = { 9, 7, 5, 5, 5, 3, 1 };
    int target = 5;
    size_t index = 999;

    TEST_CHECK(rarr_binSearch_desc(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 4);

    target = 4;
    TEST_CHECK(!rarr_binSearch_desc(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 5);

    return 0;
}

static int test_rarr_bin_search_desc_returns_descending_insertion_index(void)
{
    int arr[] = { 9, 7, 5, 3, 1 };
    int target = 6;
    size_t index = 999;

    TEST_CHECK(!rarr_binSearch_desc(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 2);

    target = 10;
    TEST_CHECK(!rarr_binSearch_desc(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == 0);

    target = 0;
    TEST_CHECK(!rarr_binSearch_desc(arr, sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]), rcmp_int32, &target, &index));
    TEST_CHECK(index == sizeof(arr) / sizeof(arr[0]));

    return 0;
}

static int test_rarr_bin_search_returns_zero_for_empty_array(void)
{
    int target = 1;
    size_t index = 999;

    TEST_CHECK(!rarr_binSearch(NULL, sizeof(target), 0, rcmp_int32, &target, &index));
    TEST_CHECK(index == 0);

    index = 999;
    TEST_CHECK(!rarr_binSearch_desc(NULL, sizeof(target), 0, rcmp_int32, &target, &index));
    TEST_CHECK(index == 0);

    return 0;
}

int test_rarr(void)
{
    TEST_RUN(test_rarr_bin_search_finds_existing_ascending_value);
    TEST_RUN(test_rarr_bin_search_returns_ascending_insertion_index);
    TEST_RUN(test_rarr_bin_search_returns_leftmost_duplicate);
    TEST_RUN(test_rarr_bin_search_desc_finds_existing_descending_value);
    TEST_RUN(test_rarr_bin_search_desc_returns_rightmost_duplicate);
    TEST_RUN(test_rarr_bin_search_desc_returns_descending_insertion_index);
    TEST_RUN(test_rarr_bin_search_returns_zero_for_empty_array);

    return 0;
}
