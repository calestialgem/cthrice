// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.cc"

namespace cthrice
{
/* Nonowning range. */
template<typename Element>
struct View {
    /* Border before the first element. */
    Element* first;
    /* Border after the last element. */
    Element* last;
};

/* Amount of elements. */
template<typename Element>
[[nodiscard]] constexpr ix size(View<Element> const& view) noexcept
{
    return view.last - view.first;
}
} // namespace cthrice
