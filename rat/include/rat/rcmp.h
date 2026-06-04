// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RCMP_H_
#define RAT_RCMP_H_

#include <rat/common.h>


// Compares left and right and returns a negative value, zero, or a positive
// value. Only the sign is part of the contract.
typedef int (*rcmp)(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);


// Compares buffer sizes first. If sizes match, compares the buffer bytes.
int rcmp_buf(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);

// Compares size-delimited strings lexicographically. Embedded NUL bytes are
// compared like any other byte.
int rcmp_str(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);

// Compares NUL-terminated strings. Size arguments are ignored.
int rcmp_cstr(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);

// Compares fixed-size numeric objects of the named type. Size arguments are
// ignored. Floating-point comparisons require non-NaN values.

int rcmp_uint8(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_uint16(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_uint32(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_uint64(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_int8(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_int16(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_int32(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_int64(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_float32(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);
int rcmp_float64(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize);

// Pointer variants load one pointer from each argument, then compare the
// pointed-to objects with the matching non-pointer comparator.

int rcmp_buf_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_str_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_cstr_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_uint8_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_uint16_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_uint32_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_uint64_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_int8_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_int16_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_int32_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_int64_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_float32_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);
int rcmp_float64_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize);

#endif
