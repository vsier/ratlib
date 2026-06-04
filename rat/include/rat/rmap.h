// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RMAP_H_
#define RAT_RMAP_H_

#include <rat/ralc.h>
#include <rat/rhash.h>


// Computes a key hash. hash_p is initialized with key0/key1 from rmap_init.
// Keys that compare equal must produce the same full hash value.
typedef rhash_val (*rmap_fn_hash)(const void *key_p, size_t keySize, rhash *hash_p);
// Compares a lookup key with a map-owned stored key.
typedef bool (*rmap_fn_equals)(const void *restrict key_p, size_t keySize, const void *restrict otherKey_p, size_t otherKeySize);

struct rmap_elemHeader;

// Hash map (dictionary) with copied variable-size keys and variable-size values.
typedef struct
{
    ralc_iface *bucketsRalc;
    ralc_iface *elemsRalc;
    struct rmap_elemHeader **buckets;
    rmap_fn_hash hash;
    rmap_fn_equals equals;
    rhash_val hashKey0;
    rhash_val hashKey1;
    size_t bucketCount;
    size_t count;
} rmap;


// Initializes an empty map. Bucket storage is allocated lazily on first insert.
// bucketsRalc_p stores bucket arrays; elemsRalc_p stores copied key/value
// elements.
void rmap_init(rmap *p, rhash_val key0, rhash_val key1, rmap_fn_hash opt_hash_fn, rmap_fn_equals opt_equals_fn, ralc_iface *bucketsRalc_p, ralc_iface *elemsRalc_p);

// Returns the elemsRalc_p request size for one copied key/value element.
size_t rmap_elemAllocSize(size_t keySize, size_t valueSize);

// Inserts or replaces key and returns map-owned value storage to initialize.
// Existing value contents are not part of the replacement contract.
void *rmap_set(rmap *p, const void *key_p, size_t keySize, size_t valueSize);
// Returns the value for key, or NULL when key is not present.
void *rmap_get(const rmap *p, const void *key_p, size_t keySize);
// Returns true if key was present in the map. If not, inserts key and returns false.
// out_value_p receives the value for key or NULL if key insertion failed.
bool rmap_getOrAdd(rmap *p, const void *key_p, size_t keySize, size_t valueSize, void **out_value_p);
// Removes key and returns whether it was present.
bool rmap_remove(rmap *p, const void *key_p, size_t keySize);

// Frees all map-owned allocations and leaves p as an initialized empty map.
void rmap_clear(rmap *p);


// Iterator over every map element. Modifying the map outside this iterator
// invalidates it.
typedef struct
{
    rmap *map;
    size_t bucketIndex;
    struct rmap_elemHeader *next;
    struct rmap_elemHeader **prevNext;
    struct rmap_elemHeader *last;
    struct rmap_elemHeader **lastPrevNext;
} rmap_iterAll;

rmap_iterAll rmap_getAll(rmap *p);
// Returns the next value, or NULL at the end. out_key_p and out_keySize_p receive
// the map-owned key for the returned value.
void *rmap_iterAll_next(rmap_iterAll *p, const void **out_key_p, size_t *out_keySize_p);
// Removes the element most recently returned by rmap_iterAll_next.
void rmap_iterAll_removeElem(rmap_iterAll *p);

#endif
