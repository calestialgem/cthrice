// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace cthrice
{
/* Dynamic storage of contiguous elements. */
template<typename Element>
struct List {
private:

    /* Border before the first element. */
    Element* first;
    /* Border after the last element. */
    Element* last;
    /* Border after the last allocated element. */
    Element* lastAllocated;
};
} // namespace cthrice
