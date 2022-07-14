// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace cthrice
{
/* Place of an element. */
template<typename Element>
struct Place {
    /* Pointer to the element at the place. */
    Element* contained;
};
} // namespace cthrice
