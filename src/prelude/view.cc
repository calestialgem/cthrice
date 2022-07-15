// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace cthrice
{
/* Immutable view of contiguous elements. */
template<typename Element>
struct View {
    /* Border at the begining. */
    Element* left;
    /* Border at the end. */
    Element* right;
};
} // namespace cthrice
