// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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

/* Border before the first element of the view. */
template<typename Element>
[[nodiscard]] Element* first(View<Element> const& view) noexcept
{
    return view.first;
}

/* Border after the last element of the view. */
template<typename Element>
[[nodiscard]] Element* last(View<Element> const& view) noexcept
{
    return view.last;
}
} // namespace cthrice
