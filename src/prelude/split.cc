// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/range.cc"
#include "prelude/view.cc"

namespace cthrice
{
/* Parts of a range, which was splitted at a border. */
template<typename Element>
struct Split {
    /* Part of the range before the split. */
    View<Element> before;
    /* Part of the range after the split. */
    View<Element> after;
};

/* Split the range at the border, which sits at the index. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr Split<Element>
split(RangeType const& range, ix const index) noexcept
{
    expect(index >= 0 && index < size(range), "Index out of bounds!");
    return {
        .before = {        .first = first(range), .last = first(range) + index},
        .after  = {.first = first(range) + index,          .last = last(range)}
    };
}
} // namespace cthrice
