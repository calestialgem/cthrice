// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/error.cc"
#include "prelude/scalar.cc"

namespace cthrice
{
    /* Pointers to contiguously placed elements. */
    template<typename Element>
    struct slice {
        /* Create pointing to a null terminated array. */
        [[nodiscard]] constexpr slice(Element* nta) noexcept
            : bgn{nta}
            , end{nta}
        {
            while (*end) {
                end++;
            }
        }

        /* Amount of elements. */
        [[nodiscard]] constexpr ix size() const noexcept
        {
            ix sze = end - bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether there are any elements. */
        [[nodiscard]] constexpr b8 finite() const noexcept
        {
            return size() > 0;
        }

        /* Whether the given slice have the same elements in the same order. */
        [[nodiscard]] constexpr b8
        equals(slice<Element> const& rhs) const noexcept
        {
            ix sze = size();
            if (sze != rhs.size()) {
                return false;
            }
            for (ix i = 0; i < sze; i++) {
                if (bgn[i] != rhs.bgn[i]) {
                    return false;
                }
            }
            return true;
        }

    private:

        /* Pointer to the first element. */
        Element* bgn;
        /* Pointer to the element after the last one. */
        Element* end;
    };
} // namespace cthrice
