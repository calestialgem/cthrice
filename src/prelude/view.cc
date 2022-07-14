// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

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
} // namespace cthrice
