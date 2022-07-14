// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace cthrice
{
/* Border between contiguously placed elements. */
template<typename Element>
struct Border {
    /* Pointer to the element after the border. */
    Element* after;
};
} // namespace cthrice
