// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rhash.h>
#include <assert.h>
#include <string.h>


// HalfSipHash-1-3-32. Whole words are loaded through memcpy; hash values assume
// little-endian object representation.

#define CROUNDS 1
#define DROUNDS 3

#define ROTL(_v, _n) (((_v) << (_n)) | ((_v) >> (sizeof(rhash_val) * CHAR_BIT - (_n))))

static void sipround(rhash_val *restrict v0_p, rhash_val *restrict v1_p, rhash_val *restrict v2_p, rhash_val *restrict v3_p)
{
    *v0_p += *v1_p;
    *v1_p = ROTL(*v1_p, 5) ^ *v0_p;
    *v0_p = ROTL(*v0_p, 16);

    *v2_p += *v3_p;
    *v3_p = ROTL(*v3_p, 8) ^ *v2_p;

    *v0_p += *v3_p;
    *v3_p = ROTL(*v3_p, 7) ^ *v0_p;

    *v2_p += *v1_p;
    *v1_p = ROTL(*v1_p, 13) ^ *v2_p;

    *v2_p = ROTL(*v2_p, 16);
}


void rhash_init(rhash *out, rhash_val key0, rhash_val key1)
{
    assert(out);

    out->val[0] = key0;
    out->val[1] = key1;
    out->val[2] = key0 ^ 0x6c796765;
    out->val[3] = key1 ^ 0x74656462;
    out->rem = 0;
    out->totalSize = 0;
}
void rhash_add(rhash *p, const void *buf, size_t bufSize)
{
    assert(p && (buf || !bufSize) && bufSize <= SIZE_MAX - p->totalSize);

    rhash_val v0 = p->val[0];
    rhash_val v1 = p->val[1];
    rhash_val v2 = p->val[2];
    rhash_val v3 = p->val[3];
    rhash_val word = p->rem;

    const unsigned char *words = (const unsigned char *)buf;

    unsigned char oldRemSize = (unsigned char)(p->totalSize % sizeof(rhash_val));
    p->totalSize += bufSize;
    if (oldRemSize)
    {
        unsigned char needSize = (unsigned char)(sizeof(rhash_val) - oldRemSize);
        if (needSize > bufSize)
        {
            for (unsigned char i = 0; i < bufSize; ++i)
                word |= (rhash_val)words[i] << ((oldRemSize + i) * CHAR_BIT);
            p->rem = word;
            return;
        }

        for (unsigned char i = 0; i < needSize; ++i)
            word |= (rhash_val)words[i] << ((oldRemSize + i) * CHAR_BIT);

        v3 ^= word;
        for (unsigned char j = 0; j < CROUNDS; ++j)
            sipround(&v0, &v1, &v2, &v3);
        v0 ^= word;

        bufSize -= needSize;
        words += needSize;
    }

    size_t wordCount = bufSize / sizeof(rhash_val);
    unsigned char remSize = (unsigned char)(bufSize % sizeof(rhash_val));

    for (size_t i = 0; i < wordCount; ++i)
    {
        memcpy(&word, words + i * sizeof(word), sizeof(word));

        v3 ^= word;
        for (unsigned char j = 0; j < CROUNDS; ++j)
            sipround(&v0, &v1, &v2, &v3);
        v0 ^= word;
    }

    p->val[0] = v0;
    p->val[1] = v1;
    p->val[2] = v2;
    p->val[3] = v3;
    p->rem = 0;
    words += wordCount * sizeof(rhash_val);
    for (unsigned char i = 0; i < remSize; ++i)
        p->rem |= (rhash_val)words[i] << (i * CHAR_BIT);
}
rhash_val rhash_result(rhash *p)
{
    assert(p);

    rhash_val v0 = p->val[0];
    rhash_val v1 = p->val[1];
    rhash_val v2 = p->val[2];
    rhash_val v3 = p->val[3];
    rhash_val word = p->rem;

    word |= (rhash_val)p->totalSize << ((sizeof(rhash_val) - 1) * CHAR_BIT);

    v3 ^= word;
    for (unsigned char j = 0; j < CROUNDS; ++j)
        sipround(&v0, &v1, &v2, &v3);
    v0 ^= word;

    v2 ^= 0xFF;
    for (unsigned char j = 0; j < DROUNDS; ++j)
        sipround(&v0, &v1, &v2, &v3);

    return v1 ^ v3;
}
rhash_val rhash_calc(rhash_val key0, rhash_val key1, const void *buf, size_t bufSize)
{
    assert(buf || !bufSize);

    rhash h;
    rhash_init(&h, key0, key1);
    rhash_add(&h, buf, bufSize);
    return rhash_result(&h);
}
