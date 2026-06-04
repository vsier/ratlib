// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <rat/rhash.h>


// Keep ROTL matching rat/src/rhash.c so copied_round stays easy to compare.
#define ROTL(_v, _n) (((_v) << (_n)) | ((_v) >> (sizeof(rhash_val) * CHAR_BIT - (_n))))

static void copied_round(rhash_val *restrict v0_p, rhash_val *restrict v1_p, rhash_val *restrict v2_p, rhash_val *restrict v3_p)
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

static void expected_compress(rhash_val *v0_p, rhash_val *v1_p, rhash_val *v2_p, rhash_val *v3_p, rhash_val word)
{
    *v3_p ^= word;
    copied_round(v0_p, v1_p, v2_p, v3_p);
    *v0_p ^= word;
}

static rhash_val expected_word_le(const unsigned char *data)
{
    return ((rhash_val)data[0]) |
        ((rhash_val)data[1] << 8) |
        ((rhash_val)data[2] << 16) |
        ((rhash_val)data[3] << 24);
}

static rhash_val expected_rhash_calc(rhash_val key0, rhash_val key1, const unsigned char *data, size_t size)
{
    rhash_val v0 = key0;
    rhash_val v1 = key1;
    rhash_val v2 = key0 ^ 0x6c796765u;
    rhash_val v3 = key1 ^ 0x74656462u;

    size_t wordCount = size / sizeof(rhash_val);
    for (size_t i = 0; i < wordCount; ++i)
        expected_compress(&v0, &v1, &v2, &v3, expected_word_le(data + i * sizeof(rhash_val)));

    rhash_val tail = (rhash_val)size << 24;
    data += wordCount * sizeof(rhash_val);
    switch (size % sizeof(rhash_val))
    {
        case 3: tail |= (rhash_val)data[2] << 16;
        case 2: tail |= (rhash_val)data[1] << 8;
        case 1: tail |= (rhash_val)data[0];
        default: break;
    }

    expected_compress(&v0, &v1, &v2, &v3, tail);
    v2 ^= 0xFFu;
    copied_round(&v0, &v1, &v2, &v3);
    copied_round(&v0, &v1, &v2, &v3);
    copied_round(&v0, &v1, &v2, &v3);

    return v1 ^ v3;
}

static int test_rhash_streaming_matches_one_shot(void)
{
    static const unsigned char data[] =
    {
        0x00, 0x01, 0x7F, 0x80, 0xFF, 0x22, 0x33, 0x44, 0x55,
        0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD
    };

    rhash_val expected = expected_rhash_calc(0x01234567u, 0x89ABCDEFu, data, sizeof(data));

    TEST_CHECK(rhash_calc(0x01234567u, 0x89ABCDEFu, data, sizeof(data)) == expected);

    for (size_t split = 0; split <= sizeof(data); ++split)
    {
        rhash h;
        rhash_init(&h, 0x01234567u, 0x89ABCDEFu);
        rhash_add(&h, data, split);
        rhash_add(&h, data + split, sizeof(data) - split);
        TEST_CHECK(rhash_result(&h) == expected);
    }

    return 0;
}

static int test_rhash_repeated_small_chunks_match_one_shot(void)
{
    static const unsigned char data[] =
    {
        0x80, 0x01, 0x02, 0x03, 0xFE, 0x04, 0x05, 0x06, 0xFD, 0x07
    };

    rhash h;
    rhash_init(&h, 1u, 2u);
    for (size_t i = 0; i < sizeof(data); ++i)
        rhash_add(&h, data + i, 1);

    TEST_CHECK(rhash_result(&h) == expected_rhash_calc(1u, 2u, data, sizeof(data)));
    TEST_CHECK(rhash_calc(1u, 2u, data, sizeof(data)) == expected_rhash_calc(1u, 2u, data, sizeof(data)));
    return 0;
}

int test_rhash(void)
{
    TEST_RUN(test_rhash_streaming_matches_one_shot);
    TEST_RUN(test_rhash_repeated_small_chunks_match_one_shot);

    return 0;
}
