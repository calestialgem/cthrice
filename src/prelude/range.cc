// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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
        } -> std::convertible_to<Element*>;
    {
        range.end
        } -> std::convertible_to<Element*>;
};

/* Call the consumer for each element in the range. */
template<typename Element, Range<Element> RangeType, typename Consumer>
constexpr void for_each(RangeType const& range, Consumer consumer) noexcept
{
    for (Element* i = range.begin; i < range.end; i++) {
        consumer(*i);
    }
}

/* Amount of elements. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr ix size(RangeType const& range) noexcept
{
    ix size = range.end - range.begin;
    expect(size >= 0, "Range has negative size!");
    return size;
}

/* Whether there are any elements. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr b8 finite(RangeType const& range) noexcept
{
    return size(range) > 0;
}

/* Pointer to the value at the index. Returns null if the index is invalid. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr Element*
get(RangeType const& range, ix const index) noexcept
{
    if (index >= 0 && index < size(range)) {
        return range.begin + index;
    }
    return nullptr;
}

/* Reference to the value at the index. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr Element&
at(RangeType const& range, ix const index) noexcept
{
    Element* pointer = get(range, index);
    expect(pointer != nullptr, "Index out of range bounds!");
    return *pointer;
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
    for (ix index = 0; index < size; index++) {
        if (at(left, index) != at(right, index)) {
            return false;
        }
    }
    return true;
}
} // namespace cthrice
