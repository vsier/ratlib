// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include "test.h"

#include <rat/ralcGen.h>
#include <rat/ralcPool.h>
#include <rat/rmap.h>
#include <string.h>


static union
{
    size_t align;
    void *ptr;
    unsigned char bytes[8192];
} buf;

typedef struct
{
    int a;
    int b;
    int c;
} bigValue;

static rhash_val constantHash(const void *key_p, size_t keySize, rhash *hash_p)
{
    (void)key_p;
    (void)keySize;
    (void)hash_p;

    return 1;
}

static bool exactEquals(const void *restrict key_p, size_t keySize, const void *restrict otherKey_p, size_t otherKeySize)
{
    return keySize == otherKeySize && memcmp(key_p, otherKey_p, keySize) == 0;
}

static bool firstByteEquals(const void *restrict key_p, size_t keySize, const void *restrict otherKey_p, size_t otherKeySize)
{
    const unsigned char *keyBytes = key_p;
    const unsigned char *otherKeyBytes = otherKey_p;

    return keySize && otherKeySize && keyBytes[0] == otherKeyBytes[0];
}

static size_t countingEquals_count;

static rhash_val firstByteHash(const void *key_p, size_t keySize, rhash *hash_p)
{
    (void)keySize;
    (void)hash_p;

    return *(const unsigned char *)key_p;
}

static bool countingExactEquals(const void *restrict key_p, size_t keySize, const void *restrict otherKey_p, size_t otherKeySize)
{
    ++countingEquals_count;
    return exactEquals(key_p, keySize, otherKey_p, otherKeySize);
}

static int test_rmap_set_copies_keys_and_gets_values(void)
{
    ralcGen parent;
    rmap map;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, NULL, NULL, &parent.ifaceRalc, &parent.ifaceRalc);
    TEST_CHECK(map.buckets == NULL);
    TEST_CHECK(map.bucketCount == 0);

    char key[] = "ab";
    int *value_p = rmap_set(&map, key, sizeof(key) - 1, sizeof(*value_p));
    TEST_CHECK(value_p != NULL);
    *value_p = 42;

    key[0] = 'x';

    int *found_p = rmap_get(&map, "ab", 2);
    TEST_CHECK(found_p != NULL);
    TEST_CHECK(*found_p == 42);
    TEST_CHECK(rmap_get(&map, key, sizeof(key) - 1) == NULL);
    TEST_CHECK(map.count == 1);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_set_replaces_key_and_value_storage(void)
{
    ralcGen parent;
    rmap map;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, constantHash, firstByteEquals, &parent.ifaceRalc, &parent.ifaceRalc);

    int *small_p = rmap_set(&map, "a", 1, sizeof(*small_p));
    TEST_CHECK(small_p != NULL);
    *small_p = 7;

    bigValue *big_p = rmap_set(&map, "apple", 5, sizeof(*big_p));
    TEST_CHECK(big_p != NULL);
    TEST_CHECK(map.count == 1);
    big_p->a = 11;
    big_p->b = 22;
    big_p->c = 33;

    // Verify that replacement copied the new key, not just the new value size.
    map.equals = exactEquals;

    bigValue *found_p = rmap_get(&map, "apple", 5);
    TEST_CHECK(found_p != NULL);
    TEST_CHECK(found_p->a == 11);
    TEST_CHECK(found_p->b == 22);
    TEST_CHECK(found_p->c == 33);
    TEST_CHECK(rmap_get(&map, "a", 1) == NULL);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_get_or_add_reports_found_state(void)
{
    ralcGen parent;
    rmap map;
    void *value_p;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, NULL, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

    TEST_CHECK(!rmap_getOrAdd(&map, "key", 3, sizeof(int), &value_p));
    TEST_CHECK(value_p != NULL);
    *(int *)value_p = 123;

    void *sameValue_p;
    TEST_CHECK(rmap_getOrAdd(&map, "key", 3, sizeof(int), &sameValue_p));
    TEST_CHECK(sameValue_p == value_p);
    TEST_CHECK(*(int *)sameValue_p == 123);
    TEST_CHECK(map.count == 1);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_remove_unlinks_colliding_elements(void)
{
    ralcGen parent;
    rmap map;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, constantHash, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

    int *a_p = rmap_set(&map, "a", 1, sizeof(*a_p));
    int *b_p = rmap_set(&map, "b", 1, sizeof(*b_p));
    int *c_p = rmap_set(&map, "c", 1, sizeof(*c_p));
    TEST_CHECK(a_p != NULL);
    TEST_CHECK(b_p != NULL);
    TEST_CHECK(c_p != NULL);
    *a_p = 1;
    *b_p = 2;
    *c_p = 3;

    TEST_CHECK(rmap_remove(&map, "b", 1));
    TEST_CHECK(map.count == 2);
    TEST_CHECK(rmap_get(&map, "b", 1) == NULL);

    a_p = rmap_get(&map, "a", 1);
    c_p = rmap_get(&map, "c", 1);
    TEST_CHECK(a_p != NULL);
    TEST_CHECK(c_p != NULL);
    TEST_CHECK(*a_p == 1);
    TEST_CHECK(*c_p == 3);
    TEST_CHECK(!rmap_remove(&map, "missing", 7));

    rmap_clear(&map);
    return 0;
}

static int test_rmap_skips_equals_when_full_hash_differs(void)
{
    ralcGen parent;
    rmap map;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, firstByteHash, countingExactEquals, &parent.ifaceRalc, &parent.ifaceRalc);

    unsigned char key1 = 1;
    unsigned char key5 = 5;

    int *value_p = rmap_set(&map, &key1, sizeof(key1), sizeof(*value_p));
    TEST_CHECK(value_p != NULL);
    *value_p = 15;

    countingEquals_count = 0;
    TEST_CHECK(rmap_get(&map, &key5, sizeof(key5)) == NULL);
    TEST_CHECK(countingEquals_count == 0);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_clear_releases_backing_allocations_and_allows_reuse(void)
{
    ralcGen parent;
    rmap map;
    ralc_usage usage;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, NULL, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

    for (int i = 0; i < 8; ++i)
    {
        int *value_p = rmap_set(&map, &i, sizeof(i), sizeof(*value_p));
        TEST_CHECK(value_p != NULL);
        *value_p = i * 10;
    }

    ralcGen_getUsage(&parent, &usage);
    TEST_CHECK(usage.blockCount > 0);
    TEST_CHECK(map.buckets != NULL);
    TEST_CHECK(map.bucketCount > 0);

    rmap_clear(&map);
    TEST_CHECK(map.buckets == NULL);
    TEST_CHECK(map.bucketCount == 0);
    TEST_CHECK(map.count == 0);

    ralcGen_getUsage(&parent, &usage);
    TEST_CHECK(usage.blockCount == 0);
    TEST_CHECK(usage.blockSize == 0);

    int key = 99;
    int *value_p = rmap_set(&map, &key, sizeof(key), sizeof(*value_p));
    TEST_CHECK(value_p != NULL);
    *value_p = 990;

    value_p = rmap_get(&map, &key, sizeof(key));
    TEST_CHECK(value_p != NULL);
    TEST_CHECK(*value_p == 990);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_iter_all_handles_empty_map(void)
{
    ralcGen parent;
    rmap map;
    rmap_iterAll iter;
    const void *key_p;
    size_t keySize;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, NULL, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

    iter = rmap_getAll(&map);
    TEST_CHECK(rmap_iterAll_next(&iter, &key_p, &keySize) == NULL);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_iter_all_visits_all_elements(void)
{
    ralcGen parent;
    rmap map;
    bool seen[6] = { 0 };
    size_t seenCount = 0;

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, firstByteHash, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

    for (unsigned char i = 0; i < sizeof(seen) / sizeof(seen[0]); ++i)
    {
        int *value_p = rmap_set(&map, &i, sizeof(i), sizeof(*value_p));
        TEST_CHECK(value_p != NULL);
        *value_p = i * 10;
    }

    rmap_iterAll iter = rmap_getAll(&map);
    const void *key_p;
    size_t keySize;
    for (int *value_p; (value_p = rmap_iterAll_next(&iter, &key_p, &keySize));)
    {
        unsigned char key = *(const unsigned char *)key_p;
        TEST_CHECK(keySize == sizeof(key));
        TEST_CHECK(key < sizeof(seen) / sizeof(seen[0]));
        TEST_CHECK(!seen[key]);
        TEST_CHECK(*value_p == key * 10);

        seen[key] = true;
        ++seenCount;
    }

    TEST_CHECK(seenCount == sizeof(seen) / sizeof(seen[0]));
    for (size_t i = 0; i < sizeof(seen) / sizeof(seen[0]); ++i)
        TEST_CHECK(seen[i]);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_iter_all_remove_elem_unlinks_current_element(void)
{
    ralcGen parent;
    rmap map;
    const char keys[] = { 'a', 'b', 'c', 'd' };

    ralcGen_init(&parent, buf.bytes, sizeof(buf.bytes));
    rmap_init(&map, 1, 2, constantHash, NULL, &parent.ifaceRalc, &parent.ifaceRalc);

    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i)
    {
        int *value_p = rmap_set(&map, keys + i, sizeof(keys[i]), sizeof(*value_p));
        TEST_CHECK(value_p != NULL);
        *value_p = keys[i];
    }

    rmap_iterAll iter = rmap_getAll(&map);
    const void *key_p;
    size_t keySize;
    for (int *value_p; (value_p = rmap_iterAll_next(&iter, &key_p, &keySize));)
    {
        char key = *(const char *)key_p;
        TEST_CHECK(keySize == sizeof(key));
        TEST_CHECK(*value_p == key);

        if (key != 'a')
            rmap_iterAll_removeElem(&iter);
    }

    TEST_CHECK(map.count == 1);
    TEST_CHECK(rmap_get(&map, "a", 1) != NULL);
    TEST_CHECK(rmap_get(&map, "b", 1) == NULL);
    TEST_CHECK(rmap_get(&map, "c", 1) == NULL);
    TEST_CHECK(rmap_get(&map, "d", 1) == NULL);

    rmap_clear(&map);
    return 0;
}

static int test_rmap_uses_separate_bucket_and_element_allocators(void)
{
    union
    {
        size_t align;
        void *ptr;
        unsigned char bytes[1024];
    } bucketBuf;
    union
    {
        size_t align;
        void *ptr;
        unsigned char bytes[2048];
    } elemBuf;

    ralcGen bucketParent;
    ralcGen elemParent;
    ralcPool elemPool;
    rmap map;
    ralc_usage bucketUsage;
    ralc_usage elemUsage;

    ralcGen_init(&bucketParent, bucketBuf.bytes, sizeof(bucketBuf.bytes));
    ralcGen_init(&elemParent, elemBuf.bytes, sizeof(elemBuf.bytes));
    ralcPool_init(&elemPool, rmap_elemAllocSize(sizeof(int), sizeof(int)), 4, &elemParent.ifaceRalc);
    rmap_init(&map, 1, 2, NULL, NULL, &bucketParent.ifaceRalc, &elemPool.ifaceRalc);

    for (int i = 0; i < 3; ++i)
    {
        int *value_p = rmap_set(&map, &i, sizeof(i), sizeof(*value_p));
        TEST_CHECK(value_p != NULL);
        *value_p = i + 10;
    }

    ralcGen_getUsage(&bucketParent, &bucketUsage);
    ralcPool_getUsage(&elemPool, &elemUsage);
    TEST_CHECK(bucketUsage.blockCount == 1);
    TEST_CHECK(elemUsage.blockCount == 3);
    TEST_CHECK(elemPool.count == 3);

    rmap_clear(&map);

    ralcGen_getUsage(&bucketParent, &bucketUsage);
    ralcPool_getUsage(&elemPool, &elemUsage);
    TEST_CHECK(bucketUsage.blockCount == 0);
    TEST_CHECK(elemUsage.blockCount == 0);
    TEST_CHECK(elemPool.count == 0);

    ralcPool_clear(&elemPool);
    return 0;
}

int test_rmap(void)
{
    TEST_RUN(test_rmap_set_copies_keys_and_gets_values);
    TEST_RUN(test_rmap_set_replaces_key_and_value_storage);
    TEST_RUN(test_rmap_get_or_add_reports_found_state);
    TEST_RUN(test_rmap_remove_unlinks_colliding_elements);
    TEST_RUN(test_rmap_skips_equals_when_full_hash_differs);
    TEST_RUN(test_rmap_clear_releases_backing_allocations_and_allows_reuse);
    TEST_RUN(test_rmap_iter_all_handles_empty_map);
    TEST_RUN(test_rmap_iter_all_visits_all_elements);
    TEST_RUN(test_rmap_iter_all_remove_elem_unlinks_current_element);
    TEST_RUN(test_rmap_uses_separate_bucket_and_element_allocators);

    return 0;
}
