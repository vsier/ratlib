// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include "test.h"

#include <rat/rorder.h>
#include <string.h>


static int test_rorder_converts_between_host_and_storage_order(void)
{
    uint16_t v16 = UINT16_C(0x1234);
    uint32_t v32 = UINT32_C(0x12345678);
    uint64_t v64 = UINT64_C(0x0123456789abcdef);

#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    TEST_CHECK(rorder_le16(v16) == v16);
    TEST_CHECK(rorder_le32(v32) == v32);
    TEST_CHECK(rorder_le64(v64) == v64);
    TEST_CHECK(rorder_be16(v16) == rorder_flip16(v16));
    TEST_CHECK(rorder_be32(v32) == rorder_flip32(v32));
    TEST_CHECK(rorder_be64(v64) == rorder_flip64(v64));
#else
    TEST_CHECK(rorder_le16(v16) == rorder_flip16(v16));
    TEST_CHECK(rorder_le32(v32) == rorder_flip32(v32));
    TEST_CHECK(rorder_le64(v64) == rorder_flip64(v64));
    TEST_CHECK(rorder_be16(v16) == v16);
    TEST_CHECK(rorder_be32(v32) == v32);
    TEST_CHECK(rorder_be64(v64) == v64);
#endif

    return 0;
}

static int test_rorder_flips_values(void)
{
    TEST_CHECK(rorder_flip16(UINT16_C(0x1234)) == UINT16_C(0x3412));
    TEST_CHECK(rorder_flip32(UINT32_C(0x12345678)) == UINT32_C(0x78563412));
    TEST_CHECK(rorder_flip64(UINT64_C(0x0123456789abcdef)) == UINT64_C(0xefcdab8967452301));

    return 0;
}

static int test_rorder_reads_from_unaligned_byte_storage(void)
{
    unsigned char le16[] = { 0, 0x34, 0x12, 0 };
    unsigned char be16[] = { 0, 0x12, 0x34, 0 };
    unsigned char le32[] = { 0, 0x78, 0x56, 0x34, 0x12, 0 };
    unsigned char be32[] = { 0, 0x12, 0x34, 0x56, 0x78, 0 };
    unsigned char le64[] = { 0, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0 };
    unsigned char be64[] = { 0, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0 };
    uint16_t out16;
    uint32_t out32;
    uint64_t out64;

    TEST_CHECK(rorder_le16_read(le16 + 1, &out16) == 2);
    TEST_CHECK(out16 == UINT16_C(0x1234));
    TEST_CHECK(rorder_be16_read(be16 + 1, &out16) == 2);
    TEST_CHECK(out16 == UINT16_C(0x1234));

    TEST_CHECK(rorder_le32_read(le32 + 1, &out32) == 4);
    TEST_CHECK(out32 == UINT32_C(0x12345678));
    TEST_CHECK(rorder_be32_read(be32 + 1, &out32) == 4);
    TEST_CHECK(out32 == UINT32_C(0x12345678));

    TEST_CHECK(rorder_le64_read(le64 + 1, &out64) == 8);
    TEST_CHECK(out64 == UINT64_C(0x0123456789abcdef));
    TEST_CHECK(rorder_be64_read(be64 + 1, &out64) == 8);
    TEST_CHECK(out64 == UINT64_C(0x0123456789abcdef));

    return 0;
}

static int test_rorder_writes_to_unaligned_byte_storage(void)
{
    unsigned char buf[10];
    unsigned char le16[] = { 0x34, 0x12 };
    unsigned char be16[] = { 0x12, 0x34 };
    unsigned char le32[] = { 0x78, 0x56, 0x34, 0x12 };
    unsigned char be32[] = { 0x12, 0x34, 0x56, 0x78 };
    unsigned char le64[] = { 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01 };
    unsigned char be64[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    memset(buf, 0, sizeof(buf));
    TEST_CHECK(rorder_le16_write(buf + 1, UINT16_C(0x1234)) == 2);
    TEST_CHECK(memcmp(buf + 1, le16, sizeof(le16)) == 0);
    TEST_CHECK(rorder_be16_write(buf + 1, UINT16_C(0x1234)) == 2);
    TEST_CHECK(memcmp(buf + 1, be16, sizeof(be16)) == 0);

    TEST_CHECK(rorder_le32_write(buf + 1, UINT32_C(0x12345678)) == 4);
    TEST_CHECK(memcmp(buf + 1, le32, sizeof(le32)) == 0);
    TEST_CHECK(rorder_be32_write(buf + 1, UINT32_C(0x12345678)) == 4);
    TEST_CHECK(memcmp(buf + 1, be32, sizeof(be32)) == 0);

    TEST_CHECK(rorder_le64_write(buf + 1, UINT64_C(0x0123456789abcdef)) == 8);
    TEST_CHECK(memcmp(buf + 1, le64, sizeof(le64)) == 0);
    TEST_CHECK(rorder_be64_write(buf + 1, UINT64_C(0x0123456789abcdef)) == 8);
    TEST_CHECK(memcmp(buf + 1, be64, sizeof(be64)) == 0);

    return 0;
}

static int test_rorder_flip_read_and_write_use_host_order_storage(void)
{
    uint16_t v16 = UINT16_C(0x1234);
    uint32_t v32 = UINT32_C(0x12345678);
    uint64_t v64 = UINT64_C(0x0123456789abcdef);
    unsigned char buf[10];
    uint16_t out16;
    uint32_t out32;
    uint64_t out64;

    memcpy(buf + 1, &v16, sizeof(v16));
    TEST_CHECK(rorder_flip16_read(buf + 1, &out16) == 2);
    TEST_CHECK(out16 == rorder_flip16(v16));
    TEST_CHECK(rorder_flip16_write(buf + 1, v16) == 2);
    memcpy(&out16, buf + 1, sizeof(out16));
    TEST_CHECK(out16 == rorder_flip16(v16));

    memcpy(buf + 1, &v32, sizeof(v32));
    TEST_CHECK(rorder_flip32_read(buf + 1, &out32) == 4);
    TEST_CHECK(out32 == rorder_flip32(v32));
    TEST_CHECK(rorder_flip32_write(buf + 1, v32) == 4);
    memcpy(&out32, buf + 1, sizeof(out32));
    TEST_CHECK(out32 == rorder_flip32(v32));

    memcpy(buf + 1, &v64, sizeof(v64));
    TEST_CHECK(rorder_flip64_read(buf + 1, &out64) == 8);
    TEST_CHECK(out64 == rorder_flip64(v64));
    TEST_CHECK(rorder_flip64_write(buf + 1, v64) == 8);
    memcpy(&out64, buf + 1, sizeof(out64));
    TEST_CHECK(out64 == rorder_flip64(v64));

    return 0;
}

int test_rorder(void)
{
    TEST_RUN(test_rorder_converts_between_host_and_storage_order);
    TEST_RUN(test_rorder_flips_values);
    TEST_RUN(test_rorder_reads_from_unaligned_byte_storage);
    TEST_RUN(test_rorder_writes_to_unaligned_byte_storage);
    TEST_RUN(test_rorder_flip_read_and_write_use_host_order_storage);

    return 0;
}
