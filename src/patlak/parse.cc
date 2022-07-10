// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/types.cc"
#include "prelude/view.cc"

namespace cthrice::patlak
{
    /* Parse tree of a pattern. */
    struct Tree {
    };

    /* Parse the pattern. */
    [[nodiscard]] Tree parse(View<const B8> ptrn)
    {
        return {};
    }

    /*  */
} // namespace cthrice::patlak
