// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <cstdint>

namespace cthrice
{
/* Index type. Pointer arithmetic should be done with this type. It is
 * better than using an unsigned type like size_t. If the pointer difference
 * cannot fit into ptrdiff_t, it is undefined behaviour anyways. */
using ix = std::ptrdiff_t;

/* Signed, 8-bit integer. */
using i8 = std::int8_t;
/* Signed, 16-bit integer. */
using i16 = std::int16_t;
/* Signed, 32-bit integer. */
using i32 = std::int32_t;
/* Signed, 64-bit integer. */
using i64 = std::int64_t;

/* Unsigned, 8-bit integer. */
using u8 = std::uint8_t;
/* Unsigned, 16-bit integer. */
using u16 = std::uint16_t;
/* Unsigned, 32-bit integer. */
using u32 = std::uint32_t;
/* Unsigned, 64-bit integer. */
using u64 = std::uint64_t;

/* Floating point, 32-bit real. */
using f32 = float;
/* Floating point, 64-bit real. */
using f64 = double;

/* Smallest memory unit, which is a signed, 8-bit integer. */
using c8 = char;

/* Boolean algebra type, which is an 8-bit integer. */
using b8 = bool;
} // namespace cthrice
