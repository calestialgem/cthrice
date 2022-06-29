// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_VIEW_HH
#define CTHRICE_VIEW_HH 1

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cthrice
{
    /** Immutable view of a range of data. */
    template<typename T>
    struct View {
        const T* bgn;
        const T* end;
    };

    /** Get the amount of bytes. */
    template<typename T>
    size_t size(View<T> view)
    {
        return view.end - view.bgn;
    }

    /** Check if the data pointed by the views are the same. */
    template<typename T>
    bool equals(View<T> lhs, View<T> rhs)
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
    const T* find(View<T> view, P pred)
    {
        for (const T* i = view.bgn; i < view.end; i++) {
            if (pred(*i)) {
                return i;
            }
        }
        return view.end;
    }
} // namespace cthrice

#endif // CTHRICE_VIEW_HH
