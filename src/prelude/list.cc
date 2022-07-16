// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace cthrice
{
/* Dynamic array. */
template<typename Element>
struct List {
    /* Border before the first element. */
    Element* first;
    /* Border after the last element. */
    Element* last;
    /* Border after the last allocated element. */
    Element* allocated;
};

/* Border before the first element of the list. */
template<typename Element>
[[nodiscard]] Element* first(List<Element> const& list) noexcept
{
    return list.first;
}

/* Border after the last element of the list. */
template<typename Element>
[[nodiscard]] Element* last(List<Element> const& list) noexcept
{
    return list.last;
}
} // namespace cthrice
