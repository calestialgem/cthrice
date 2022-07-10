// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "types.cc"

namespace cthrice
{
    /* View of contiguously placed elements. */
    template<typename T>
    struct View {
        /* Underlying type with correct qualifiers. */
        using Type = T;

        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the end. */
        T* end;
    };

    namespace view
    {
        /* View of a null terminated array. */
        template<typename T>
        View<T> terminated(T* nta)
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
        bool equal(View<T> lhs, decltype(lhs) rhs)
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

        /* Whether the index is valid. */
        template<typename T>
        bool valid(View<T> view, Ix ix)
        {
            return ix >= 0 && ix < size(view);
        }

        /* Whether the part is valid. */
        template<typename T>
        bool valid_view(View<T> view, decltype(view) part)
        {
            return part.bgn >= view.bgn && part.end <= view.end;
        }

        /* Reference to the value at the index. */
        template<typename T>
        T& at(View<T> view, Ix ix)
        {
            debug(valid(view, ix), "Invalid index!");
            return view.bgn[ix];
        }

        /* Pointer to the value at the index. Returns null if the index is
         * invalid. */
        template<typename T>
        T* get(View<T> view, Ix ix)
        {
            if (valid(view, ix)) {
                return view.bgn + ix;
            }
            return nullptr;
        }

        /* Parts of a view, which was splitted at a point. The element where the
         * split happens is the first element of the after view. */
        template<typename T>
        struct Split {
            /* Part of the split from the begining. */
            View<T> before;
            /* Part of the split from the end. */
            decltype(before) after;
        };

        /* Split the view from the given position. */
        template<typename T>
        Split<T> split(View<T> view, typename decltype(view)::Type* pos)
        {
            debug(pos >= view.bgn && pos <= view.end, "Invalid position!");
            return {
                .before = {.bgn = view.bgn,      .end = pos},
                .after  = {     .bgn = pos, .end = view.end}
            };
        }

        /* View of a part of the view from the element at the begin index to the
         * element one before the end index. */
        template<typename T>
        View<T> view_part(View<T> view, Ix bix, Ix eix)
        {
            View<T> res = {.bgn = view.bgn + bix, .end = view.bgn + eix};
            debug(valid_view(view, res), "Creating invalid view!");
            return res;
        }

        /* View of the begining of the view from the first element to the
         * element one before the end index. */
        template<typename T>
        View<T> view_trail(View<T> view, Ix eix)
        {
            View<T> res = {.bgn = view.bgn, .end = view.bgn + eix};
            debug(valid_view(view, res), "Creating invalid view!");
            return res;
        }

        /* View of the end of the view from the element at the begin index upto
         * the last valid element. */
        template<typename T>
        View<T> view_tail(View<T> view, Ix bix)
        {
            View<T> res = {.bgn = view.bgn + bix, .end = view.end};
            debug(valid_view(view, res), "Creating invalid view!");
            return res;
        }

        /* Find the first position of the element that fits the predicate.
         * Returns the pointer to the element after the last one if none of the
         * elements fits the predicate. */
        template<typename T, typename P>
        T* first_fit(View<T> view, P pred)
        {
            T* res = view.bgn;
            while (res < view.end && !pred(*res)) {
                res++;
            }
            return res;
        }

        /* Find the last position of the element that fits the predicate.
         * Returns the pointer to the element before the first one if none of
         * the elements fits the predicate. */
        template<typename T, typename P>
        T* last_fit(View<T> view, P pred)
        {
            T* res = view.end - 1;
            while (res >= view.bgn && !pred(*res)) {
                res--;
            }
            return res;
        }

        /* Whether there is an element that fits the predicate. */
        template<typename T, typename P>
        bool contains_fit(View<T> view, P pred)
        {
            return first_fit(view, pred) != view.end;
        }

        /* Split the view from the first element that fits the predicate. */
        template<typename T, typename P>
        Split<T> before_fit(View<T> view, P pred)
        {
            return split(view, first_fit(view, pred));
        }

        /* Split the view from the element one after the last element that fits
         * the predicate. */
        template<typename T, typename P>
        Split<T> after_fit(View<T> view, P pred)
        {
            return split(view, last_fit(view, pred) + 1);
        }

        /* Find the first position of the element. Returns the pointer to
         * the element after the last one if it does not exist. */
        template<typename T>
        T* first(View<T> view, const typename decltype(view)::Type& t)
        {
            return first_fit(view, [t](const T& elem) {
                return elem == t;
            });
        }

        /* Find the last position of the element. Returns the pointer to
         * the element after the last one if it does not exist. */
        template<typename T>
        T* last(View<T> view, const typename decltype(view)::Type& t)
        {
            return last_fit(view, [t](const T& elem) {
                return elem == t;
            });
        }

        /* Whether the element is in. */
        template<typename T>
        bool contains(View<T> view, const typename decltype(view)::Type& t)
        {
            return first(view, t) != view.end;
        }

        /* Split the view from the first occurance of the element. */
        template<typename T>
        Split<T> before(View<T> view, const typename decltype(view)::Type& t)
        {
            return split(view, first(view, t));
        }

        /* Split the view from the element on after the last occurance of the
         * element. */
        template<typename T>
        Split<T> after(View<T> view, const typename decltype(view)::Type& t)
        {
            return split(view, last(view, t) + 1);
        }
    } // namespace view
} // namespace cthrice
