// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include <rat/rcmp.h>
#include <assert.h>
#include <string.h>


#define CMP(_left, _right) (((_left) > (_right)) - ((_left) < (_right)))
#define ASSERT_BUFS() assert((left_p || !leftSize) && (right_p || !rightSize))
#define ASSERT_CMP_OBJS() assert(left_p && right_p)
#define ASSERT_CMP_P_OBJS() assert(left_pp && right_pp)


int rcmp_buf(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_BUFS();

    int sizeCmp = CMP(leftSize, rightSize);
    if (sizeCmp) return sizeCmp;
    return memcmp(left_p, right_p, leftSize);
}
int rcmp_str(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_BUFS();

    int cmp = memcmp(left_p, right_p, leftSize < rightSize ? leftSize : rightSize);
    if (cmp) return cmp;
    return CMP(leftSize, rightSize);
}
int rcmp_cstr(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    return strcmp((const char *)left_p, (const char *)right_p);
}
int rcmp_uint8(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    uint8_t left = *(const uint8_t *)left_p, right = *(const uint8_t *)right_p;
    return CMP(left, right);
}
int rcmp_uint16(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    uint16_t left = *(const uint16_t *)left_p, right = *(const uint16_t *)right_p;
    return CMP(left, right);
}
int rcmp_uint32(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    uint32_t left = *(const uint32_t *)left_p, right = *(const uint32_t *)right_p;
    return CMP(left, right);
}
int rcmp_uint64(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    uint64_t left = *(const uint64_t *)left_p, right = *(const uint64_t *)right_p;
    return CMP(left, right);
}
int rcmp_int8(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    int8_t left = *(const int8_t *)left_p, right = *(const int8_t *)right_p;
    return CMP(left, right);
}
int rcmp_int16(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    int16_t left = *(const int16_t *)left_p, right = *(const int16_t *)right_p;
    return CMP(left, right);
}
int rcmp_int32(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    int32_t left = *(const int32_t *)left_p, right = *(const int32_t *)right_p;
    return CMP(left, right);
}
int rcmp_int64(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    int64_t left = *(const int64_t *)left_p, right = *(const int64_t *)right_p;
    return CMP(left, right);
}
int rcmp_float32(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    float left = *(const float *)left_p, right = *(const float *)right_p;
    assert(left == left && right == right);
    return CMP(left, right);
}
int rcmp_float64(const void *left_p, size_t leftSize, const void *right_p, size_t rightSize)
{
    ASSERT_CMP_OBJS();
    (void)leftSize;
    (void)rightSize;

    double left = *(const double *)left_p, right = *(const double *)right_p;
    assert(left == left && right == right);
    return CMP(left, right);
}

int rcmp_buf_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_buf(*(const void **)left_pp, leftSize, *(const void **)right_pp, rightSize);
}
int rcmp_str_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_str(*(const void **)left_pp, leftSize, *(const void **)right_pp, rightSize);
}
int rcmp_cstr_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_cstr(*(const void **)left_pp, leftSize, *(const void **)right_pp, rightSize);
}
int rcmp_uint8_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_uint8(*(const uint8_t **)left_pp, leftSize, *(const uint8_t **)right_pp, rightSize);
}
int rcmp_uint16_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_uint16(*(const uint16_t **)left_pp, leftSize, *(const uint16_t **)right_pp, rightSize);
}
int rcmp_uint32_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_uint32(*(const uint32_t **)left_pp, leftSize, *(const uint32_t **)right_pp, rightSize);
}
int rcmp_uint64_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_uint64(*(const uint64_t **)left_pp, leftSize, *(const uint64_t **)right_pp, rightSize);
}
int rcmp_int8_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_int8(*(const int8_t **)left_pp, leftSize, *(const int8_t **)right_pp, rightSize);
}
int rcmp_int16_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_int16(*(const int16_t **)left_pp, leftSize, *(const int16_t **)right_pp, rightSize);
}
int rcmp_int32_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_int32(*(const int32_t **)left_pp, leftSize, *(const int32_t **)right_pp, rightSize);
}
int rcmp_int64_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_int64(*(const int64_t **)left_pp, leftSize, *(const int64_t **)right_pp, rightSize);
}
int rcmp_float32_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_float32(*(const float **)left_pp, leftSize, *(const float **)right_pp, rightSize);
}
int rcmp_float64_p(const void *left_pp, size_t leftSize, const void *right_pp, size_t rightSize)
{
    ASSERT_CMP_P_OBJS();
    return rcmp_float64(*(const double **)left_pp, leftSize, *(const double **)right_pp, rightSize);
}
