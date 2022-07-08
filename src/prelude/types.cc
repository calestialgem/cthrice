// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <cstdint>

namespace cthrice
{
    /* Hash type. */
    using Hash = std::uint64_t;

    /* Index type. Pointer arithmetic should be done with this type. It is
     * better than using an unsigned type like size_t. If the pointer difference
     * cannot fit into ptrdiff_t, it is undefined behaviour anyways. */
    using Ix = std::ptrdiff_t;

    /* Signed, 8-bit integer. */
    using I8 = std::int8_t;
    /* Signed, 16-bit integer. */
    using I16 = std::int16_t;
    /* Signed, 32-bit integer. */
    using I32 = std::int32_t;
    /* Signed, 64-bit integer. */
    using I64 = std::int64_t;

    /* Unsigned, 8-bit integer. */
    using U8 = std::uint8_t;
    /* Unsigned, 16-bit integer. */
    using U16 = std::uint16_t;
    /* Unsigned, 32-bit integer. */
    using U32 = std::uint32_t;
    /* Unsigned, 64-bit integer. */
    using U64 = std::uint64_t;

    /* Floating point, 32-bit real. */
    using F32 = float;
    /* Floating point, 64-bit real. */
    using F64 = double;

    /* Smallest memory unit, which is a signed, 8-bit integer. */
    using B8 = char;
} // namespace cthrice
