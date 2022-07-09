// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/types.cc"
#include "prelude/view.cc"

namespace cthrice::map
{
    /* Hash a pattern name. Optimized for names that are made of characters in
     * English alphabet and underscores. */
    Hash hash(View<B8> name)
    {
        Hash res = 0;

        for (const B8* i = name.bgn; i < name.end; i++) {
            constexpr Hash prime = 53;
            res *= prime;
            res += *i;
        }

        return res;
    }

    /* Whether the pattern names are the same. */
    bool equal(View<B8> lhs, View<B8> rhs)
    {
        return view::equal(lhs, rhs);
    }
} // namespace cthrice::map
