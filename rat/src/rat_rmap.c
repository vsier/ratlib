// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include <rat/rmap.h>
#include <rat/extra.h>
#include <assert.h>
#include <string.h>


// Each element is one allocation:
// +--------+-----+-------+
// | Header | Key | Value |
// +--------+-----+-------+
typedef struct rmap_elemHeader
{
    struct rmap_elemHeader *next;
    rhash_val hashVal;
    size_t keySize;
} elemHeader;

#define INIT_BUCKET_COUNT 4
#define IS_POW2(_x) ((_x) && !((_x) & ((_x) - 1)))
#define BUCKETS_VALID(_p) (!(_p)->bucketCount || (IS_POW2((_p)->bucketCount) && (_p)->buckets))
#define MAP_VALID(_p) ((_p) && (_p)->bucketsRalc && (_p)->elemsRalc && BUCKETS_VALID(_p))

// Element byte layout helpers.
#define ELEM_SIZE(_keySize, _valueSize) (RAT_ALIGN(sizeof(elemHeader)) + RAT_ALIGN(_keySize) + RAT_ALIGN(_valueSize))
#define ELEM_KEY(_header_p)             ((unsigned char *)(_header_p) + ELEM_SIZE(0, 0))
#define ELEM_VALUE(_header_p)           ((unsigned char *)(_header_p) + ELEM_SIZE(_header_p->keySize, 0))


static rhash_val defaultHash(const void *key_p, size_t keySize, rhash *hash_p)
{
    assert((key_p || !keySize) && hash_p);

    rhash_add(hash_p, key_p, keySize);
    return rhash_result(hash_p);
}
static bool defaultEquals(const void *restrict key_p, size_t keySize, const void *restrict otherKey_p, size_t otherKeySize)
{
    assert((key_p || !keySize) && (otherKey_p || !otherKeySize));

    return keySize == otherKeySize && memcmp(key_p, otherKey_p, keySize) == 0;
}


static rhash_val getHashVal(const rmap *p, const void *key_p, size_t keySize)
{
    assert(p && p->hash && (key_p || !keySize));

    rhash hash;
    rhash_init(&hash, p->hashKey0, p->hashKey1);
    return p->hash(key_p, keySize, &hash);
}


static bool rehash(rmap *p, size_t newBucketCount)
{
    assert(MAP_VALID(p) && IS_POW2(newBucketCount) &&
        newBucketCount <= SIZE_MAX / sizeof(*p->buckets));

    // Bucket counts are powers of two, so hash-to-bucket uses a mask.
    size_t newBucketsSize = sizeof(*p->buckets) * newBucketCount;
    size_t actualSize;
    elemHeader **newBuckets = ralc_alloc(p->bucketsRalc, newBucketsSize, &actualSize);
    if (!newBuckets) return false;
    assert(actualSize >= newBucketsSize);

    memset(newBuckets, 0, newBucketsSize);

    if (p->bucketCount)
    {
        for (size_t i = 0; i < p->bucketCount; ++i)
        {
            for (elemHeader *next_p, *elem_p = p->buckets[i]; elem_p; elem_p = next_p)
            {
                next_p = elem_p->next;

                size_t bucketIndex = elem_p->hashVal & (newBucketCount - 1);
                elem_p->next = newBuckets[bucketIndex];
                newBuckets[bucketIndex] = elem_p;
            }
        }

        ralc_free(p->bucketsRalc, p->buckets);
    }
    p->buckets = newBuckets;
    p->bucketCount = newBucketCount;
    return true;
}

static void *add(rmap *p, rhash_val hashVal, const void *key_p, size_t keySize, size_t valueSize)
{
    assert(MAP_VALID(p) && (key_p || !keySize));

    // The zero-bucket state represents a lazily initialized or cleared map.
    if (p->count >= p->bucketCount / 2 + p->bucketCount / 4)
    {
        if (!rehash(p, p->bucketCount ? p->bucketCount * 2 : INIT_BUCKET_COUNT)) return NULL;
    }

    size_t bucketIndex = hashVal & (p->bucketCount - 1);

    elemHeader *newElem_p = ralc_alloc(p->elemsRalc, ELEM_SIZE(keySize, valueSize), RAT_TMP_PTR(size_t, 1));
    if (!newElem_p) return NULL;
    newElem_p->next = p->buckets[bucketIndex];
    newElem_p->hashVal = hashVal;
    newElem_p->keySize = keySize;
    memcpy(ELEM_KEY(newElem_p), key_p, keySize);

    p->buckets[bucketIndex] = newElem_p;
    ++p->count;
    return ELEM_VALUE(newElem_p);
}


void rmap_init(rmap *p, rhash_val key0, rhash_val key1, rmap_fn_hash opt_hash_fn, rmap_fn_equals opt_equals_fn, ralc_iface *bucketsRalc_p, ralc_iface *elemsRalc_p)
{
    assert(p && bucketsRalc_p && elemsRalc_p);

    p->bucketsRalc = bucketsRalc_p;
    p->elemsRalc = elemsRalc_p;
    p->buckets = NULL;
    p->hash = opt_hash_fn ? opt_hash_fn : defaultHash;
    p->equals = opt_equals_fn ? opt_equals_fn : defaultEquals;
    p->hashKey0 = key0;
    p->hashKey1 = key1;
    p->bucketCount = 0;
    p->count = 0;
}

void *rmap_set(rmap *p, const void *key_p, size_t keySize, size_t valueSize)
{
    assert(MAP_VALID(p) && (key_p || !keySize));

    rhash_val hashVal = getHashVal(p, key_p, keySize);

    if (p->bucketCount)
    {
        size_t bucketIndex = hashVal & (p->bucketCount - 1);

        elemHeader **prevNext_pp = p->buckets + bucketIndex;
        for (elemHeader *elem_p = p->buckets[bucketIndex]; elem_p; elem_p = elem_p->next)
        {
            if (elem_p->hashVal == hashVal && p->equals(key_p, keySize, ELEM_KEY(elem_p), elem_p->keySize))
            {
                size_t newElemSize = ELEM_SIZE(keySize, valueSize);
                elemHeader *newElem_p = ralc_realloc(p->elemsRalc, elem_p, newElemSize, RAT_TMP_PTR(size_t, 1));
                if (!newElem_p)
                {
                    newElem_p = ralc_alloc(p->elemsRalc, newElemSize, RAT_TMP_PTR(size_t, 1));
                    if (!newElem_p) return NULL;
                    newElem_p->next = elem_p->next;
                    newElem_p->hashVal = hashVal;
                    ralc_free(p->elemsRalc, elem_p);
                }
                newElem_p->keySize = keySize;
                memcpy(ELEM_KEY(newElem_p), key_p, keySize);

                *prevNext_pp = newElem_p;
                return ELEM_VALUE(newElem_p);
            }
            prevNext_pp = &elem_p->next;
        }
    }
    return add(p, hashVal, key_p, keySize, valueSize);
}

void *rmap_get(const rmap *p, const void *key_p, size_t keySize)
{
    assert(MAP_VALID(p) && (key_p || !keySize));

    if (p->bucketCount)
    {
        rhash_val hashVal = getHashVal(p, key_p, keySize);
        size_t bucketIndex = hashVal & (p->bucketCount - 1);

        for (elemHeader *elem_p = p->buckets[bucketIndex]; elem_p; elem_p = elem_p->next)
        {
            if (elem_p->hashVal == hashVal && p->equals(key_p, keySize, ELEM_KEY(elem_p), elem_p->keySize))
                return ELEM_VALUE(elem_p);
        }
    }
    return NULL;
}

bool rmap_getOrAdd(rmap *p, const void *key_p, size_t keySize, size_t valueSize, void **out_value_p)
{
    assert(MAP_VALID(p) && (key_p || !keySize) && out_value_p);

    rhash_val hashVal = getHashVal(p, key_p, keySize);

    if (p->bucketCount)
    {
        size_t bucketIndex = hashVal & (p->bucketCount - 1);

        for (elemHeader *elem_p = p->buckets[bucketIndex]; elem_p; elem_p = elem_p->next)
        {
            if (elem_p->hashVal == hashVal && p->equals(key_p, keySize, ELEM_KEY(elem_p), elem_p->keySize))
            {
                *out_value_p = ELEM_VALUE(elem_p);
                return true;
            }
        }
    }

    *out_value_p = add(p, hashVal, key_p, keySize, valueSize);
    return false;
}

bool rmap_remove(rmap *p, const void *key_p, size_t keySize)
{
    assert(MAP_VALID(p) && (key_p || !keySize));

    if (p->bucketCount)
    {
        rhash_val hashVal = getHashVal(p, key_p, keySize);
        size_t bucketIndex = hashVal & (p->bucketCount - 1);

        elemHeader **prevNext_pp = p->buckets + bucketIndex;
        for (elemHeader *elem_p = p->buckets[bucketIndex]; elem_p; elem_p = elem_p->next)
        {
            if (elem_p->hashVal == hashVal && p->equals(key_p, keySize, ELEM_KEY(elem_p), elem_p->keySize))
            {
                *prevNext_pp = elem_p->next;
                ralc_free(p->elemsRalc, elem_p);
                --p->count;
                return true;
            }
            prevNext_pp = &elem_p->next;
        }
    }
    return false;
}

size_t rmap_elemAllocSize(size_t keySize, size_t valueSize)
{
    assert(RAT_ALIGNMENT > 0);
    assert(keySize <= SIZE_MAX - (RAT_ALIGNMENT - 1));
    assert(valueSize <= SIZE_MAX - (RAT_ALIGNMENT - 1));
    assert(RAT_ALIGN(sizeof(elemHeader)) <= SIZE_MAX - RAT_ALIGN(keySize));
    assert(RAT_ALIGN(sizeof(elemHeader)) + RAT_ALIGN(keySize) <= SIZE_MAX - RAT_ALIGN(valueSize));

    return ELEM_SIZE(keySize, valueSize);
}

void rmap_clear(rmap *p)
{
    assert(p && MAP_VALID(p));

    if (!p->bucketCount) return;

    for (size_t i = 0; i < p->bucketCount; ++i)
    {
        for (elemHeader *next_p, *elem_p = p->buckets[i]; elem_p; elem_p = next_p)
        {
            next_p = elem_p->next;
            ralc_free(p->elemsRalc, elem_p);
        }
    }
    ralc_free(p->bucketsRalc, p->buckets);
    p->buckets = NULL;
    p->bucketCount = 0;
    p->count = 0;
}


rmap_iterAll rmap_getAll(rmap *p)
{
    assert(MAP_VALID(p));

    return (rmap_iterAll){
        .map = p,
        .bucketIndex = 0,
        .next = NULL,
        .prevNext = NULL,
        .last = NULL,
        .lastPrevNext = NULL,
    };
}

void *rmap_iterAll_next(rmap_iterAll *p, const void **out_key_p, size_t *out_keySize_p)
{
    assert(p && MAP_VALID(p->map) && out_key_p && out_keySize_p);

    p->last = NULL;
    p->lastPrevNext = NULL;

    while (!p->next && p->bucketIndex < p->map->bucketCount)
    {
        p->prevNext = p->map->buckets + p->bucketIndex;
        p->next = *p->prevNext;
        ++p->bucketIndex;
    }

    if (!p->next) return NULL;

    elemHeader *elem_p = p->next;
    p->last = elem_p;
    p->lastPrevNext = p->prevNext;
    p->next = elem_p->next;
    p->prevNext = &elem_p->next;

    *out_key_p = ELEM_KEY(elem_p);
    *out_keySize_p = elem_p->keySize;
    return ELEM_VALUE(elem_p);
}

void rmap_iterAll_removeElem(rmap_iterAll *p)
{
    assert(p && MAP_VALID(p->map) && p->last && p->lastPrevNext && p->map->count);

    elemHeader *elem_p = p->last;
    *p->lastPrevNext = elem_p->next;
    p->prevNext = p->lastPrevNext;
    p->last = NULL;
    p->lastPrevNext = NULL;

    ralc_free(p->map->elemsRalc, elem_p);
    --p->map->count;
}
