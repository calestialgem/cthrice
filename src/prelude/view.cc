// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

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
} // namespace cthrice
