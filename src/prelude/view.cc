// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.cc"

#include <ostream>

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

/* Stream the elements to the standard output. */
template<typename Element>
constexpr std::ostream&
operator<<(std::ostream& out, View<Element> const& view) noexcept
{
    for (Element* i = view.first; i < view.last; i++) {
        out << *i;
    }
    return out;
}
} // namespace cthrice
