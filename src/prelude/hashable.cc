// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/equality_comparible.cc"
#include "prelude/scalar.cc"

#include <concepts>

namespace cthrice
{
/* Convertible to a number, called hash. Must be equality comparible. Equal
 * objects should give the same hash. */
template<typename Type>
concept Hashable = EqualityComparible<Type> && requires(Type const& hashable)
{
    {
        hash(hashable)
        } -> std::same_as<u64>;
};
} // namespace cthrice
