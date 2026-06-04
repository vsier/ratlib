// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_RBIT_H_
#define RAT_RBIT_H_

#include <rat/common.h>


// Bit helpers for integer flag and field values. Operations are expressed in
// uintmax_t width; callers should pass unsigned integer values. Bit offsets
// and counts are valid only when their shifts are valid for uintmax_t. Use
// RBIT_MASK_FULL for the full-width mask instead of RBIT_MASK(0, width).

// Converts between byte counts and bit counts.
#define RBIT_BITSIZE(_byteSize) ((_byteSize) * CHAR_BIT)
#define RBIT_BYTESIZE(_bitSize) (((_bitSize) + CHAR_BIT - 1) / CHAR_BIT)

// Single-bit mask.
#define RBIT(_bitOffset) (UINTMAX_C(1) << (_bitOffset))

// Single-flag helpers.
#define RBIT_FLAG(_bitOffset, _value)           ((_value) == 0 ? 0 : RBIT(_bitOffset))
#define RBIT_FLAG_GET(_x, _bitOffset)           (((_x) & RBIT(_bitOffset)) != 0)
#define RBIT_FLAG_SET(_x, _bitOffset)           ((_x) | RBIT(_bitOffset))
#define RBIT_FLAG_UNSET(_x, _bitOffset)         ((_x) & ~RBIT(_bitOffset))
#define RBIT_FLAG_WRITE(_x, _bitOffset, _value) (RBIT_FLAG_UNSET(_x, _bitOffset) | RBIT_FLAG(_bitOffset, _value))

// Returns flags unchanged for readability at call sites.
#define RBIT_FLAGS(_flags)           (_flags)
// Returns the bits from x that are present in flags.
#define RBIT_FLAGS_GET(_x, _flags)   ((_x) & (_flags))
// Returns x with all bits from flags set.
#define RBIT_FLAGS_SET(_x, _flags)   ((_x) | (_flags))
// Returns x with all bits from flags cleared.
#define RBIT_FLAGS_UNSET(_x, _flags) ((_x) & ~(_flags))
// Returns whether x contains exactly flags.
#define RBIT_FLAGS_EQUAL(_x, _flags) ((_x) == (_flags))
// Returns whether x contains every bit from flags.
#define RBIT_FLAGS_ALL(_x, _flags)   (((_x) & (_flags)) == (_flags))
// Returns whether x has no bits outside flags.
#define RBIT_FLAGS_ONLY(_x, _flags)  (((_x) & (_flags)) == (_x))
// Returns whether x contains at least one bit from flags.
#define RBIT_FLAGS_ANY(_x, _flags)   (((_x) & (_flags)) != 0)

// Contiguous bit masks. _bitCount must be smaller than uintmax_t width for
// RBIT_MASK; RBIT_MASK_FULL represents the full-width mask.
#define RBIT_MASK(_bitOffset, _bitCount)      ((RBIT(_bitCount) - 1) << (_bitOffset))
#define RBIT_MASK_FULL                        UINTMAX_MAX

// Contiguous bit-field helpers. Bits outside the field are preserved by WRITE.
#define RBIT_FIELD(_bitOffset, _bitCount, _value)           (((_value) & RBIT_MASK(0, _bitCount)) << (_bitOffset))
#define RBIT_FIELD_GET(_x, _bitOffset, _bitCount)           (((_x) >> (_bitOffset)) & RBIT_MASK(0, _bitCount))
#define RBIT_FIELD_SET(_x, _bitOffset, _bitCount)           ((_x) | RBIT_MASK(_bitOffset, _bitCount))
#define RBIT_FIELD_UNSET(_x, _bitOffset, _bitCount)         ((_x) & ~RBIT_MASK(_bitOffset, _bitCount))
#define RBIT_FIELD_WRITE(_x, _bitOffset, _bitCount, _value) (RBIT_FIELD_UNSET(_x, _bitOffset, _bitCount) | RBIT_FIELD(_bitOffset, _bitCount, _value))

#endif
