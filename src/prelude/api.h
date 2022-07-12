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

/* If the condition does not hold, print the error message and abort with the
 * file and line number. */
void expect_source_location(b8 cnd, c8 const* msg, c8 const* file, u32 line);

/* If the condition does not hold, print the error message and abort with the
 * file and line number. Provides source location by itself. */
#define expect(cnd, msg) expect_source_location(cnd, msg, __FILE__, __LINE__)

/* Print the error message and abort with the file and line number. */
void unexpected_source_location(c8 const* msg, c8 const* file, u32 line);

/* Print the error message and abort with the file and line number. Provides
 * source location by itself. */
#define unexpected(msg) unexpected_source_location(msg, __FILE__, __LINE__)
