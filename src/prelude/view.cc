// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "types.cc"

namespace cthrice
{
    /* Immutable view of contiguously placed elements. */
    template<typename T>
    struct View {
        /* Pointer to the element at the begining. */
        const T* bgn;
        /* Pointer to the element one after the end. */
        const T* end;
    };

    namespace view
    {
        /* View of a null terminated array. */
        template<typename T>
        View<T> terminated(const T* nta)
        {
            View<T> res = {.bgn = nta, .end = nta};
            while (*res.end) {
                res.end++;
            }
            return res;
        }

        /* Amount of elements. */
        template<typename T>
        Ix size(View<T> view)
        {
            Ix sze = view.end - view.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether there are any elements. */
        template<typename T>
        bool finite(View<T> view)
        {
            return size(view) > 0;
        }

        /* Whether the views have the same elements in the same order. */
        template<typename T>
        bool equal(View<T> lhs, View<T> rhs)
        {
            Ix sze = size(lhs);
            if (sze != size(rhs)) {
                return false;
            }
            for (Ix i = 0; i < sze; i++) {
                if (lhs.bgn[i] != rhs.bgn[i]) {
                    return false;
                }
            }
            return true;
        }

        /* Find the first position of the element that fits the predicate.
         * Returns the pointer to the element after the last one if none of the
         * elements fits the predicate. */
        template<typename T, typename P>
        const T* find_fit(View<T> view, P pred)
        {
            const T* res = view.bgn;
            while (res < view.end && !pred(*res)) {
                res++;
            }
            return res;
        }

        /* Whether there is an element that fits the predicate. */
        template<typename T, typename P>
        bool contains_fit(View<T> view, P pred)
        {
            return find_fit(view, pred) != view.end;
        }

        /* Find the first position of the element. Returns the pointer to the
         * element after the last one if it does not exist. */
        template<typename T>
        const T* find(View<T> view, T t)
        {
            return find_fit(view, [t](T elem) {
                return elem == t;
            });
        }

        /* Whether the element is in. */
        template<typename T>
        bool contains(View<T> view, T t)
        {
            return find(view, t) != view.end;
        }
    } // namespace view
} // namespace cthrice
