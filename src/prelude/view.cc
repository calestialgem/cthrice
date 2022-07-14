// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "border.cc"

namespace cthrice
{
/* Nonowning view of another range. */
template<typename Element>
struct View {
    /* Border at the begining. */
    Border<Element> begin;
    /* Border at the end. */
    Border<Element> end;
};
} // namespace cthrice
