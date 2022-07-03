// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TYPES_H
#define TYPES_H 1

#include <stddef.h>
#include <stdint.h>

/* Type that pointer arithmetic should be done with. Better than using an
 * unsigned type like size_t. If the pointer difference cannot fit this, it is
 * undefined behaviour any ways. */
typedef ptrdiff_t ptr;

/* Pointer that does not point to anything. */
extern const ptr null;

/* Signed 8-bit integer. */
typedef int8_t i8;
/* Signed 16-bit integer. */
typedef int16_t i16;
/* Signed 32-bit integer. */
typedef int32_t i32;
/* Signed 64-bit integer. */
typedef int64_t i64;

/* Unsigned 8-bit integer. */
typedef uint8_t u8;
/* Unsigned 16-bit integer. */
typedef uint16_t u16;
/* Unsigned 32-bit integer. */
typedef uint32_t u32;
/* Unsigned 64-bit integer. */
typedef uint64_t u64;

/* 32-bit floating point number. */
typedef float f32;
/* 64-bit floating point number. */
typedef double f64;

/* Integer that can be 1 or 0. */
typedef _Bool bool;

/* True boolean value; 1. */
extern const bool true;

/* False boolean value; 0. */
extern const bool false;

/* Smallest memory unit. */
typedef char byte;

#endif // TYPES_H
