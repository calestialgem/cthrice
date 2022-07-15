// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.cc"

#include <array>

namespace cthrice
{
/* Nonowning view of another range. */
template<typename Element>
struct View {
    /* Border at the begining. */
    Element* begin;
    /* Border at the end. */
    Element* end;
};

/* View of a null terminated array. */
template<typename Element>
[[nodiscard]] constexpr View<Element> view_terminated(Element* array) noexcept
{
    View<Element> view = {.begin = array, .end = array};
    while (*view.end) {
        view.end++;
    }
    return view;
}

/* View of a static array. */
template<typename Element, ix Size>
[[nodiscard]] constexpr View<Element>
view_static(std::array<Element, Size>& array) noexcept
{
    return {.begin = array.data(), .end = array.data() + Size};
}
} // namespace cthrice
