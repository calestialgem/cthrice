// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Equality comparible to each other. */
template<typename Type>
concept Equalable = requires(Type const& equalable)
{
    {
        equal(equalable, equalable)
        } -> std::same_as<bool>;
};
} // namespace cthrice
