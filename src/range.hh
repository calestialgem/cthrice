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
    size_t size(Range<T> rnge)
    {
        return rnge.end - rnge.bgn;
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
    const T* find(Range<T> rnge, P pred)
    {
        for (const T* i = rnge.bgn; i < rnge.end; i++) {
            if (pred(*i)) {
                return i;
            }
        }
        return rnge.end;
    }

    /** Check whether the range contains any element that fits the given
     * predicate. */
    template<typename T, typename P>
    bool contains(Range<T> rnge, P pred)
    {
        return find(rnge, pred) != rnge.end;
    }

    /** Find the first position of the given element. Returns the position after
     * the last element if not found. */
    template<typename T>
    const T* find(Range<T> rnge, T look)
    {
        return find(rnge, [look](T elem) {
            return elem == look;
        });
    }

    /** Check whether the range contains the given element. */
    template<typename T>
    bool contains(Range<T> rnge, T look)
    {
        return contains(rnge, [look](T elem) {
            return elem == look;
        });
    }
} // namespace cthrice

#endif // CTHRICE_RANGE_HH
