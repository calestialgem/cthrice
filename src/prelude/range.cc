// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/border.cc"
#include "prelude/expect.cc"
#include "prelude/scalar.cc"

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

/* Amount of elements. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr ix size(RangeType const& range) noexcept
{
    ix size = distance(range.begin, range.end);
    expect(size >= 0, "Negative size!");
    return size;
}

/* Whether there are any elements. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr b8 finite(RangeType const& range) noexcept
{
    return size(range) > 0;
}

/* Whether the ranges have equal elements with the same order. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr b8
equal(RangeType const& left, RangeType const& right) noexcept
{
    ix size = size(left);
    if (size != size(right)) {
        return false;
    }
    for (ix i = 0; i < size; i++) {
        if (after(left.begin, i) != after(right.begin, i)) {
            return false;
        }
    }
    return true;
}
} // namespace cthrice
