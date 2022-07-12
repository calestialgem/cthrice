// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stddef.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
// --------------------------[   S  C  A  L  A  R   ]---------------------------
// -----------------------------------------------------------------------------

/* Index and pointer arithmetic integer. */
typedef ptrdiff_t ix;

/* Signed, 8-bit integer. */
typedef int8_t i8;
/* Signed, 16-bit integer. */
typedef int16_t i16;
/* Signed, 32-bit integer. */
typedef int32_t i32;
/* Signed, 64-bit integer. */
typedef int64_t i64;

/* Unsigned, 8-bit integer. */
typedef uint8_t u8;
/* Unsigned, 16-bit integer. */
typedef uint16_t u16;
/* Unsigned, 32-bit integer. */
typedef uint32_t u32;
/* Unsigned, 64-bit integer. */
typedef uint64_t u64;

/* Floating point, 32-bit real. */
typedef float f32;
/* Floating point, 64-bit real. */
typedef double f64;

/* Smallest memory unit, which is a signed, 8-bit integer. */
typedef char c8;

/* Boolean algebraic, 8-bit, integer. */
typedef _Bool b8;

/* Boolean true. */
#define true ((b8)1)

/* Boolean false. */
#define false ((b8)0)

/* Pointer to nothing. */
#define null ((void*)0)

// -----------------------------------------------------------------------------
// --------------------------[   E  X  P  E  C  T   ]---------------------------
// -----------------------------------------------------------------------------

/* If the condition does not hold, print the error message and abort with the
 * file and line number. */
void expectsl(b8 cnd, c8 const* msg, c8 const* file, u32 line);

/* If the condition does not hold, print the error message and abort with the
 * file and line number. Provides source location by itself. */
#define expect(cnd, msg) expectsl(cnd, msg, __FILE__, __LINE__)

/* Print the error message and abort with the file and line number. */
void unexpectedsl(c8 const* msg, c8 const* file, u32 line);

/* Print the error message and abort with the file and line number. Provides
 * source location by itself. */
#define unexpected(msg) unexpectedsl(msg, __FILE__, __LINE__)

// -----------------------------------------------------------------------------
// ----------------------------[   S  L  I  C  E   ]----------------------------
// -----------------------------------------------------------------------------

/* Pointers to contiguous elements. */
typedef struct {
    /* Pointer to the element at the begining. */
    c8* bgn;
    /* Pointer to the element one after the last one. */
    c8* end;
} Slice;

/* Parts of a slice, which was splitted at an element. The element where the
 * split happens is the first element of the trail. */
typedef struct {
    /* Part of the slice from the begining to the split. */
    Slice lead;
    /* Part of the slice from the split to the end. */
    Slice trail;
} Split;

/* Create pointing to the null terminated array. */
Slice slice_terminated(c8* a);

/* Amount of elements. */
ix slice_size(Slice s);

/* Whether there are any elements. */
b8 slice_finite(Slice s);

/* Whether the slices point to equal elements in the same order. */
b8 slice_equal(Slice lhs, Slice rhs);

/* Whether the position is valid. */
b8 slice_validp(Slice s, c8* p);

/* Whether the index is valid. */
b8 slice_validi(Slice s, ix i);

/* Whether the part is valid. */
b8 slice_valids(Slice s, Slice p);

/* Pointer to the value at the index. Returns null if the index is invalid. */
c8* slice_get(Slice s, ix i);

/* Value at the index. */
c8 slice_at(Slice s, ix i);

/* First position of the element that fits the predicate. Returns the pointer to
 * the element one after the last one if none of the elements fits. */
c8* slice_firstf(Slice s, b8 (*p)(c8));

/* Whether there is an element that fits the predicate. */
b8 slice_containsf(Slice s, b8 (*p)(c8));

/* First position of the element. Returns the pointer to the element one after
 * the last one if the element does not exist. */
c8* slice_firste(Slice s, c8 e);

/* Whether the element exists. */
b8 slice_containse(Slice s, c8 e);

/* Split the slice at the position. */
Split slice_splitp(Slice s, c8* p);

/* Split the slice at the index. */
Split slice_spliti(Slice s, ix i);

/* Split the slice at the first element that fits the predicate. */
Split slice_splitf(Slice s, b8 (*p)(c8));

/* Split the slice at the first occurance of element. */
Split slice_splite(Slice s, c8 e);
