// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Pointers to contiguous range of elements. */
template<typename Type, typename Element>
concept Range = requires(Type range)
{
    {
        first(range)
        } -> std::convertible_to<Element*>;
    {
        last(range)
        } -> std::convertible_to<Element*>;
};
} // namespace cthrice
