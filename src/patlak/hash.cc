// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/slice.cc"
#include "prelude/types.cc"

namespace cthrice::map
{
    /* Hash a pattern name. Optimized for names that are made of characters in
     * English alphabet and underscores. */
    Hash hash(Slice<const B8> name)
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
    bool equal(Slice<const B8> lhs, Slice<const B8> rhs)
    {
        return slice::equal(lhs, rhs);
    }
} // namespace cthrice::map
