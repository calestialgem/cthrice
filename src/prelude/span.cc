// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error.cc"
#include "types.cc"

namespace cthrice
{
    /* Immutable view of contiguously placed elements. */
    template<typename T>
    struct Span {
        /* Pointer to the element at the begining. */
        const T* bgn;
        /* Pointer to the element one after the end. */
        const T* end;

        /* View of a null terminated array. */
        static Span<T> terminated(const T* nta)
        {
            Span res = {.bgn = nta, .end = nta};
            while (*res.end) {
                res.end++;
            }
            return res;
        }

        /* Amount of elements. */
        static ix size(Span<T> span)
        {
            ix sze = span.end - span.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether there are any elements. */
        static bool finite(Span<T> span)
        {
            return size(span) > 0;
        }

        /* Whether the spans have the same elements in the same order. */
        static bool equal(Span<T> lhs, Span<T> rhs)
        {
            ix sze = size(lhs);
            if (sze != size(rhs)) {
                return false;
            }
            for (ix i = 0; i < sze; i++) {
                if (lhs.bgn[i] != rhs.bgn[i]) {
                    return false;
                }
            }
            return true;
        }

        /* Find the first position of the element that fits the predicate.
         * Returns the pointer to the element after the last one if none of the
         * elements fits the predicate. */
        template<typename P>
        static const T* find_fit(Span<T> span, P pred)
        {
            const T* res = span.bgn;
            while (res < span.end && !pred(*res)) {
                res++;
            }
            return res;
        }

        /* Whether there is an element that fits the predicate. */
        template<typename P>
        static bool contains_fit(Span<T> span, P pred)
        {
            return find_fit(span, pred) != span.end;
        }

        /* Find the first position of the element. Returns the pointer to the
         * element after the last one if it does not exist. */
        static const T* find(Span<T> span, T t)
        {
            return find_fit(span, [t](T elem) {
                return elem == t;
            });
        }

        /* Whether the element is in. */
        static bool contains(Span<T> span, T t)
        {
            return find(span, t) != span.end;
        }
    };
} // namespace cthrice
