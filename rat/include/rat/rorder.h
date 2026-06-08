// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RORDER_H_
#define RAT_RORDER_H_

#include <rat/common.h>


// Fixed-width integer byte-order helpers. le/be convert between host integer
// values and little/big-endian byte storage; flip swaps bytes unconditionally.

// Converts a 16-bit value between host order and little-endian byte order.
uint16_t rorder_le16(uint16_t value);
// Reads a little-endian 16-bit value from byte storage. Returns 2.
unsigned char rorder_le16_read(const void *src_p, uint16_t *out_value);
// Writes a 16-bit value to little-endian byte storage. Returns 2.
unsigned char rorder_le16_write(void *dst_p, uint16_t value);

// Converts a 16-bit value between host order and big-endian byte order.
uint16_t rorder_be16(uint16_t value);
// Reads a big-endian 16-bit value from byte storage. Returns 2.
unsigned char rorder_be16_read(const void *src_p, uint16_t *out_value);
// Writes a 16-bit value to big-endian byte storage. Returns 2.
unsigned char rorder_be16_write(void *dst_p, uint16_t value);

// Reverses the byte order of a 16-bit value.
uint16_t rorder_flip16(uint16_t value);
// Reads a host-order 16-bit value from byte storage, then reverses its bytes.
unsigned char rorder_flip16_read(const void *src_p, uint16_t *out_value);
// Reverses a 16-bit value's bytes, then writes it in host byte order.
unsigned char rorder_flip16_write(void *dst_p, uint16_t value);

// Converts a 32-bit value between host order and little-endian byte order.
uint32_t rorder_le32(uint32_t value);
// Reads a little-endian 32-bit value from byte storage. Returns 4.
unsigned char rorder_le32_read(const void *src_p, uint32_t *out_value);
// Writes a 32-bit value to little-endian byte storage. Returns 4.
unsigned char rorder_le32_write(void *dst_p, uint32_t value);

// Converts a 32-bit value between host order and big-endian byte order.
uint32_t rorder_be32(uint32_t value);
// Reads a big-endian 32-bit value from byte storage. Returns 4.
unsigned char rorder_be32_read(const void *src_p, uint32_t *out_value);
// Writes a 32-bit value to big-endian byte storage. Returns 4.
unsigned char rorder_be32_write(void *dst_p, uint32_t value);

// Reverses the byte order of a 32-bit value.
uint32_t rorder_flip32(uint32_t value);
// Reads a host-order 32-bit value from byte storage, then reverses its bytes.
unsigned char rorder_flip32_read(const void *src_p, uint32_t *out_value);
// Reverses a 32-bit value's bytes, then writes it in host byte order.
unsigned char rorder_flip32_write(void *dst_p, uint32_t value);

// Converts a 64-bit value between host order and little-endian byte order.
uint64_t rorder_le64(uint64_t value);
// Reads a little-endian 64-bit value from byte storage. Returns 8.
unsigned char rorder_le64_read(const void *src_p, uint64_t *out_value);
// Writes a 64-bit value to little-endian byte storage. Returns 8.
unsigned char rorder_le64_write(void *dst_p, uint64_t value);

// Converts a 64-bit value between host order and big-endian byte order.
uint64_t rorder_be64(uint64_t value);
// Reads a big-endian 64-bit value from byte storage. Returns 8.
unsigned char rorder_be64_read(const void *src_p, uint64_t *out_value);
// Writes a 64-bit value to big-endian byte storage. Returns 8.
unsigned char rorder_be64_write(void *dst_p, uint64_t value);

// Reverses the byte order of a 64-bit value.
uint64_t rorder_flip64(uint64_t value);
// Reads a host-order 64-bit value from byte storage, then reverses its bytes.
unsigned char rorder_flip64_read(const void *src_p, uint64_t *out_value);
// Reverses a 64-bit value's bytes, then writes it in host byte order.
unsigned char rorder_flip64_write(void *dst_p, uint64_t value);

#endif
