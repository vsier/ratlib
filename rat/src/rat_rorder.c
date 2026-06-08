// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rorder.h>
#include <assert.h>
#include <string.h>


#define FUNCDEF_READ(_func, _bits) \
unsigned char _func##_read(const void *src_p, uint##_bits##_t *out_value) \
{ \
    uint##_bits##_t value; \
    assert(src_p && out_value); \
    memcpy(&value, src_p, sizeof(value)); \
    *out_value = _func(value); \
    return sizeof(value); \
}

#define FUNCDEF_WRITE(_func, _bits) \
unsigned char _func##_write(void *dst_p, uint##_bits##_t value) \
{ \
    assert(dst_p); \
    value = _func(value); \
    memcpy(dst_p, &value, sizeof(value)); \
    return sizeof(value); \
}


uint16_t rorder_le16(uint16_t value)
{
#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    return value;
#else
    return rorder_flip16(value);
#endif
}
FUNCDEF_READ(rorder_le16, 16)
FUNCDEF_WRITE(rorder_le16, 16)

uint16_t rorder_be16(uint16_t value)
{
#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    return rorder_flip16(value);
#else
    return value;
#endif
}
FUNCDEF_READ(rorder_be16, 16)
FUNCDEF_WRITE(rorder_be16, 16)

uint16_t rorder_flip16(uint16_t value)
{
    return (uint16_t)((value >> 8) | (value << 8));
}
FUNCDEF_READ(rorder_flip16, 16)
FUNCDEF_WRITE(rorder_flip16, 16)


uint32_t rorder_le32(uint32_t value)
{
#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    return value;
#else
    return rorder_flip32(value);
#endif
}
FUNCDEF_READ(rorder_le32, 32)
FUNCDEF_WRITE(rorder_le32, 32)

uint32_t rorder_be32(uint32_t value)
{
#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    return rorder_flip32(value);
#else
    return value;
#endif
}
FUNCDEF_READ(rorder_be32, 32)
FUNCDEF_WRITE(rorder_be32, 32)

uint32_t rorder_flip32(uint32_t value)
{
    return ((value & UINT32_C(0x000000ff)) << 24) |
        ((value & UINT32_C(0x0000ff00)) << 8) |
        ((value & UINT32_C(0x00ff0000)) >> 8) |
        ((value & UINT32_C(0xff000000)) >> 24);
}
FUNCDEF_READ(rorder_flip32, 32)
FUNCDEF_WRITE(rorder_flip32, 32)


uint64_t rorder_le64(uint64_t value)
{
#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    return value;
#else
    return rorder_flip64(value);
#endif
}
FUNCDEF_READ(rorder_le64, 64)
FUNCDEF_WRITE(rorder_le64, 64)

uint64_t rorder_be64(uint64_t value)
{
#ifdef RAT_CONFIG_BYTEORDER_LITTLE_ENDIAN
    return rorder_flip64(value);
#else
    return value;
#endif
}
FUNCDEF_READ(rorder_be64, 64)
FUNCDEF_WRITE(rorder_be64, 64)

uint64_t rorder_flip64(uint64_t value)
{
    return ((value & UINT64_C(0x00000000000000ff)) << 56) |
        ((value & UINT64_C(0x000000000000ff00)) << 40) |
        ((value & UINT64_C(0x0000000000ff0000)) << 24) |
        ((value & UINT64_C(0x00000000ff000000)) << 8) |
        ((value & UINT64_C(0x000000ff00000000)) >> 8) |
        ((value & UINT64_C(0x0000ff0000000000)) >> 24) |
        ((value & UINT64_C(0x00ff000000000000)) >> 40) |
        ((value & UINT64_C(0xff00000000000000)) >> 56);
}
FUNCDEF_READ(rorder_flip64, 64)
FUNCDEF_WRITE(rorder_flip64, 64)


#undef FUNCDEF_WRITE
#undef FUNCDEF_READ
