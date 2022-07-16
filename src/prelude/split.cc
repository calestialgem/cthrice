// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/view.cc"

namespace cthrice
{
/* Parts of a range, which was splitted at a border. */
template<typename Element>
struct Split {
    /* Part of the range before the split. */
    View<Element> before;
    /* Part of the range after the split. */
    View<Element> after;
};
} // namespace cthrice
