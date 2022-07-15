// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/function.cc"
#include "prelude/scalar.cc"

namespace cthrice
{
/* Immutable view of contiguous elements. */
template<typename Element>
struct View {
    /* Amount of elements. */
    [[nodiscard]] constexpr ix size() const noexcept
    {
        return last - first;
    }

    /* Call the consumer for all the elements in the view. */
    template<Function<void, Element> Consumer>
    constexpr void consume(Consumer consumer) const noexcept
    {
        for (Element const* i{first}; i < last; i++) {
            consumer(*i);
        }
    }

    /* View from the pointers. */
    [[nodiscard]] constexpr View(
        Element const* const first,
        Element const* const last) noexcept
        : first{first}
        , last{last}
    {
        expect(first <= last, "Illegal view!");
    }

    /* View of a null terminated array. */
    [[nodiscard]] constexpr explicit View(Element const* const array) noexcept
        : first{array}
        , last{array}
    {
        while (*last) {
            last++;
        }
    }

private:

    /* Border before the first element. */
    Element const* first;
    /* Border after the last element. */
    Element const* last;
};
} // namespace cthrice
