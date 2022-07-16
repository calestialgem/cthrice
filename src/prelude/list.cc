// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/scalar.cc"
#include "prelude/view.cc"

#include <cstdlib>

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

/* View of the list. */
template<typename Element>
[[nodiscard]] constexpr View<Element> view(List<Element> const& list) noexcept
{
    return {.first = list.first, .last = list.last};
}

/* Amount of elements. */
template<typename Element>
[[nodiscard]] constexpr ix size(List<Element> const& list) noexcept
{
    return list.last - list.first;
}

/* Amount of allocated elements. */
template<typename Element>
[[nodiscard]] constexpr ix capacity(List<Element> const& list) noexcept
{
    return list.allocated - list.first;
}

/* Amount of allocated but unused elements. */
template<typename Element>
[[nodiscard]] constexpr ix space(List<Element> const& list) noexcept
{
    return list.allocated - list.last;
}

/* Make sure the space is at least as big as the amount. Grows if necessary by
 * at least the half of the current capacity. */
template<typename Element>
constexpr void reserve(List<Element>& list, ix amount) noexcept
{
    expect(amount >= 0, "Reserving negative amount!");
    ix growth{amount - space(list)};
    if (growth <= 0) {
        return;
    }

    ix       capacity{cthrice::capacity(list)};
    ix       newCapacity{capacity + std::max(growth, capacity >> 1)};
    Element* memory{
        (Element*)std::realloc(list.first, sizeof(Element) * newCapacity)};
    expect(memory != nullptr, "Could not allocate!");

    list.last      = memory + size(list);
    list.first     = memory;
    list.allocated = memory + newCapacity;
}

/* Deallocate the memory. */
template<typename Element>
constexpr void free(List<Element>& list) noexcept
{
    std::free(list.first);
    list.first     = nullptr;
    list.last      = nullptr;
    list.allocated = nullptr;
}
} // namespace cthrice
