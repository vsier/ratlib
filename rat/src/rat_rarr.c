// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rarr.h>
#include <assert.h>
#include <string.h>


void *rarr_insert(void *arr, size_t elemSize, size_t arrLen, size_t index, size_t count)
{
    assert(elemSize && index <= arrLen && count <= arrLen - index);

    unsigned char *b = (unsigned char *)arr + index * elemSize;
    memmove(b + count * elemSize, b, (arrLen - index - count) * elemSize);
    return b;
}
void *rarr_remove(void *arr, size_t elemSize, size_t arrLen, size_t index, size_t count)
{
    assert(elemSize && index <= arrLen && count <= arrLen - index);

    unsigned char *b = (unsigned char *)arr + index * elemSize;
    memmove(b, b + count * elemSize, (arrLen - index - count) * elemSize);
    return b;
}
void *rarr_resize(void *arr, size_t elemSize, size_t arrLen, size_t index, size_t oldCount, size_t newCount)
{
    assert(elemSize && index <= arrLen);

    if (oldCount < newCount)
    {
        assert(newCount <= arrLen - index);
        return rarr_insert(arr, elemSize, arrLen, index + oldCount, newCount - oldCount);
    }
    else
    {
        assert(oldCount <= arrLen - index);
        return rarr_remove(arr, elemSize, arrLen, index + newCount, oldCount - newCount);
    }
}

bool rarr_binSearch(const void *arr, size_t elemSize, size_t arrLen, rcmp cmp_fn, const void *target_p, size_t *out_index)
{
    assert(elemSize && (arr || !arrLen) && cmp_fn && (target_p || !arrLen) && out_index);

    const unsigned char *b = arr;
    size_t left = 0;
    size_t right = arrLen;

    while (left < right)
    {
        size_t mid = left + (right - left) / 2;

        int cmp = cmp_fn(b + mid * elemSize, elemSize, target_p, elemSize);
        if (cmp < 0)
            left = mid + 1;
        else
            right = mid;
    }

    *out_index = left;
    return left < arrLen && cmp_fn(b + left * elemSize, elemSize, target_p, elemSize) == 0;
}

bool rarr_binSearch_desc(const void *arr, size_t elemSize, size_t arrLen, rcmp cmp_fn, const void *target_p, size_t *out_index)
{
    assert(elemSize && (arr || !arrLen) && cmp_fn && (target_p || !arrLen) && out_index);

    const unsigned char *b = arr;
    size_t left = 0;
    size_t right = arrLen;

    while (left < right)
    {
        size_t mid = left + (right - left) / 2;

        int cmp = cmp_fn(b + mid * elemSize, elemSize, target_p, elemSize);
        if (cmp >= 0)
            left = mid + 1;
        else
            right = mid;
    }

    if (left > 0 && cmp_fn(b + (left - 1) * elemSize, elemSize, target_p, elemSize) == 0)
    {
        *out_index = left - 1;
        return true;
    }

    *out_index = left;
    return false;
}
