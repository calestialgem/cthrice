// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/slice.cc"
#include "prelude/types.cc"

namespace cthrice::patlak
{
    /* Parse tree of a pattern. */
    struct Tree {
    };

    /* Parse the pattern. */
    [[nodiscard]] Tree parse(Slice<const B8> ptrn)
    {
        return {};
    }

    /*  */
} // namespace cthrice::patlak
