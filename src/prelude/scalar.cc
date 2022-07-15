// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <cstdint>

namespace cthrice
{
/* Signed, 8-bit integer. */
using i8 = std::int8_t;
/* Signed, 16-bit integer. */
using i16 = std::int16_t;
/* Signed, 32-bit integer. */
using i32 = std::int32_t;
/* Signed, 64-bit integer. */
using i64 = std::int64_t;

/* Signed, 8-bit integer. */
using u8 = std::uint8_t;
/* Signed, 16-bit integer. */
using u16 = std::uint16_t;
/* Signed, 32-bit integer. */
using u32 = std::uint32_t;
/* Signed, 64-bit integer. */
using u64 = std::uint64_t;

/* Unsigned index and pointer arithmetic integer. */
using ix = std::ptrdiff_t;

/* Floating-point, 32-bit real. */
using f32 = float;
/* Floating-point, 64-bit real. */
using f64 = double;
} // namespace cthrice
