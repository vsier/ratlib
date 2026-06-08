// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_EXTRA_H_
#define RAT_EXTRA_H_

#include <rat/common.h>


// Returns sizeof member _m in struct _s.
#define RAT_M_SIZEOF(_s, _m) sizeof(((_s *)0)->_m)

// Converts a member pointer to its containing struct pointer.
#define RAT_M_TO_S(_s, _m, _mPtr) (_s *)((unsigned char *)(_mPtr) - offsetof(_s, _m))

// Compound-literal temporary array helper.
#define RAT_TMP_PTR(_t, _count) ((_t[_count]){ 0 })

// Returns the element count of an array value (not a pointer).
#define RAT_LEN(_arrVal) (sizeof(_arrVal) / sizeof((_arrVal)[0]))

struct rat_alignmentProbe
{
    char c;
    // Alignment used by the library's generic byte-storage allocators.
    union rat_alignmentStorage
    {
        void *ptr;
        void (*fn)(void);
        long l;
        long long ll;
        double d;
        long double ld;
        intmax_t imax;
        uintmax_t umax;
    } t;
};
#define RAT_ALIGNMENT offsetof(struct rat_alignmentProbe, t)

// Rounds up to RAT_ALIGNMENT. _size may be an expression.
#define RAT_ALIGN(_size) (((_size) + RAT_ALIGNMENT - 1) / RAT_ALIGNMENT * RAT_ALIGNMENT)

#endif
