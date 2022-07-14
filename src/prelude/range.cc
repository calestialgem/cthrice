// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "border.cc"

#include <concepts>

namespace cthrice
{
/* Range of contiguously placed elements. */
template<typename RangeType, typename Element>
concept Range = requires(RangeType range)
{
    {
        range.begin
        } -> std::convertible_to<Border<Element>>;
    {
        range.end
        } -> std::convertible_to<Border<Element>>;
};
} // namespace cthrice
