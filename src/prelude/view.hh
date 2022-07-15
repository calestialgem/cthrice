// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.hh"
#include "prelude/function.hh"
#include "prelude/scalar.hh"

namespace cthrice
{
/* Immutable view of contiguous elements. */
template<typename Element>
struct View {
    /* Amount of elements. */
    [[nodiscard]] constexpr ix size() const noexcept
    {
        return right - left;
    }

    /* Call the consumer for all the elements in the view. */
    template<Function<void, Element> Consumer>
    constexpr void consume(Consumer consumer) const noexcept
    {
        for (Element const* i{left}; i < right; i++) {
            consumer(*i);
        }
    }

    /* View from the pointers. */
    [[nodiscard]] constexpr View(
        Element const* const left,
        Element const* const right) noexcept
        : left{left}
        , right{right}
    {
        expect(left <= right, "Illegal view!");
    }

    /* View of a null terminated array. */
    [[nodiscard]] constexpr explicit View(Element const* const array) noexcept
        : left{array}
        , right{array}
    {
        while (*right) {
            right++;
        }
    }

private:

    /* Border at the begining. */
    Element const* left;
    /* Border at the end. */
    Element const* right;
};
} // namespace cthrice
