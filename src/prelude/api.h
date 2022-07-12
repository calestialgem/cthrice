// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stddef.h>
#include <stdint.h>

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
