// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/range.cc"
#include "prelude/scalar.cc"
#include "prelude/view.cc"

#include <array>
#include <cstddef>

namespace cthrice
{
/* Parts of a range, which was siplitted at a border. */
template<typename Element>
struct Split {
    /* Part of the range before the border. */
    View<Element> before;
    /* Part of the range after the border. */
    View<Element> after;
};

/* Split the range from the given position. */
template<typename Element, Range<Element> RangeType>
[[nodiscard]] constexpr Split<Element>
split(RangeType const& range, Element* position) noexcept
{
    expect(
        position >= range.begin && position <= range.end,
        "Position out of range bounds!");
    return {
        .before = {.begin = range.begin,  .end = position},
        .after  = {   .begin = position, .end = range.end}
    };
}
} // namespace cthrice
