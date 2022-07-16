// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Equality comparible to each other. */
template<typename Type>
concept EqualityComparible = requires(Type const& equalityComparible)
{
    {
        equal(equalityComparible, equalityComparible)
        } -> std::same_as<bool>;
};
} // namespace cthrice
