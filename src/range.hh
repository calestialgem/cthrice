// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_RANGE_HH
#define CTHRICE_RANGE_HH 1

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cthrice
{
    /** Immutable view of a range of data. */
    template<typename T>
    struct Range {
        const T* bgn;
        const T* end;
    };

    /** Get the amount of elements. */
    template<typename T>
    size_t size(Range<T> view)
    {
        return view.end - view.bgn;
    }

    /** Check if the data pointed by the ranges are the equal. */
    template<typename T>
    bool equal(Range<T> lhs, Range<T> rhs)
    {
        size_t n = size(lhs);
        if (n != size(rhs)) {
            return false;
        }
        for (size_t i = 0; i < n; i++) {
            if (lhs.bgn[i] != rhs.bgn[i]) {
                return false;
            }
        }
        return true;
    }

    /** Find the first element that fits the given predicate. Returns the
     * position after the last element if not found. */
    template<typename T, typename P>
    const T* find(Range<T> view, P pred)
    {
        for (const T* i = view.bgn; i < view.end; i++) {
            if (pred(*i)) {
                return i;
            }
        }
        return view.end;
    }
} // namespace cthrice

#endif // CTHRICE_RANGE_HH
