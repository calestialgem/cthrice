// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.cc"

namespace cthrice
{
/* Border between contiguously placed elements. */
template<typename Element>
struct Border {
    /* Pointer to the element after the border. */
    Element* after;
};

/* Distance between the borders. */
template<typename Element>
[[nodiscard]] constexpr ix
distance(Border<Element> const& from, Border<Element> const& to) noexcept
{
    return to.after - from.after;
}

/* Element that sits after the distance to the border. */
template<typename Element>
[[nodiscard]] constexpr Element&
after(Border<Element> const& border, ix const distance) noexcept
{
    return border.after[distance];
}
} // namespace cthrice
