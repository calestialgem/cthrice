// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "types.cc"

namespace cthrice
{
    /* Slice of contiguously placed elements. */
    template<typename T>
    struct Slice {
        /* Underlying type with correct qualifiers. */
        using Type = T;

        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the end. */
        T* end;
    };

    namespace slice
    {
        /* Slice of a null terminated array. */
        template<typename T>
        Slice<T> terminated(T* nta)
        {
            Slice<T> res = {.bgn = nta, .end = nta};
            while (*res.end) {
                res.end++;
            }
            return res;
        }

        /* Amount of elements. */
        template<typename T>
        Ix size(Slice<T> slice)
        {
            Ix sze = slice.end - slice.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether there are any elements. */
        template<typename T>
        bool finite(Slice<T> slice)
        {
            return size(slice) > 0;
        }

        /* Whether the slices have the same elements in the same order. */
        template<typename T>
        bool equal(Slice<T> lhs, Slice<T> rhs)
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
        bool valid(Slice<T> slice, Ix ix)
        {
            return ix >= 0 && ix < size(slice);
        }

        /* Whether the part is valid. */
        template<typename T>
        bool valid_slice(Slice<T> slice, Slice<T> part)
        {
            return part.bgn >= slice.bgn && part.end <= slice.end;
        }

        /* Reference to the value at the index. */
        template<typename T>
        T& at(Slice<T> slice, Ix ix)
        {
            debug(valid(slice, ix), "Invalid index!");
            return slice.bgn[ix];
        }

        /* Pointer to the value at the index. Returns null if the index is
         * invalid. */
        template<typename T>
        T* get(Slice<T> slice, Ix ix)
        {
            if (valid(slice, ix)) {
                return slice.bgn + ix;
            }
            return nullptr;
        }

        /* Parts of a slice, which was splitted at a point. The element where
         * the split happens is the first element of the after slice. */
        template<typename T>
        struct Split {
            /* Part of the split from the begining. */
            Slice<T> before;
            /* Part of the split from the end. */
            Slice<T> after;
        };

        /* Split the slice from the given position. */
        template<typename T>
        Split<T> split(Slice<T> slice, typename decltype(slice)::Type* pos)
        {
            debug(pos >= slice.bgn && pos <= slice.end, "Invalid position!");
            return {
                .before = {.bgn = slice.bgn,       .end = pos},
                .after  = {      .bgn = pos, .end = slice.end}
            };
        }

        /* Slice of a part of the slice from the element at the begin index to
         * the element one before the end index. */
        template<typename T>
        Slice<T> slice_part(Slice<T> slice, Ix bix, Ix eix)
        {
            Slice<T> res = {.bgn = slice.bgn + bix, .end = slice.bgn + eix};
            debug(valid_slice(slice, res), "Creating invalid slice!");
            return res;
        }

        /* Slice of the begining of the slice from the first element to the
         * element one before the end index. */
        template<typename T>
        Slice<T> slice_trail(Slice<T> slice, Ix eix)
        {
            Slice<T> res = {.bgn = slice.bgn, .end = slice.bgn + eix};
            debug(valid_slice(slice, res), "Creating invalid slice!");
            return res;
        }

        /* Slice of the end of the slice from the element at the begin index
         * upto the last valid element. */
        template<typename T>
        Slice<T> slice_tail(Slice<T> slice, Ix bix)
        {
            Slice<T> res = {.bgn = slice.bgn + bix, .end = slice.end};
            debug(valid_slice(slice, res), "Creating invalid slice!");
            return res;
        }

        /* Find the first position of the element that fits the predicate.
         * Returns the pointer to the element after the last one if none of the
         * elements fits the predicate. */
        template<typename T, typename P>
        T* first_fit(Slice<T> slice, P pred)
        {
            T* res = slice.bgn;
            while (res < slice.end && !pred(*res)) {
                res++;
            }
            return res;
        }

        /* Find the last position of the element that fits the predicate.
         * Returns the pointer to the element before the first one if none of
         * the elements fits the predicate. */
        template<typename T, typename P>
        T* last_fit(Slice<T> slice, P pred)
        {
            T* res = slice.end - 1;
            while (res >= slice.bgn && !pred(*res)) {
                res--;
            }
            return res;
        }

        /* Whether there is an element that fits the predicate. */
        template<typename T, typename P>
        bool contains_fit(Slice<T> slice, P pred)
        {
            return first_fit(slice, pred) != slice.end;
        }

        /* Split the slice from the first element that fits the predicate. */
        template<typename T, typename P>
        Split<T> before_fit(Slice<T> slice, P pred)
        {
            return split(slice, first_fit(slice, pred));
        }

        /* Split the slice from the element one after the last element that fits
         * the predicate. */
        template<typename T, typename P>
        Split<T> after_fit(Slice<T> slice, P pred)
        {
            return split(slice, last_fit(slice, pred) + 1);
        }

        /* Find the first position of the element. Returns the pointer to
         * the element after the last one if it does not exist. */
        template<typename T>
        T* first(Slice<T> slice, const typename decltype(slice)::Type& t)
        {
            return first_fit(slice, [t](const T& elem) {
                return elem == t;
            });
        }

        /* Find the last position of the element. Returns the pointer to
         * the element after the last one if it does not exist. */
        template<typename T>
        T* last(Slice<T> slice, const typename decltype(slice)::Type& t)
        {
            return last_fit(slice, [t](const T& elem) {
                return elem == t;
            });
        }

        /* Whether the element is in. */
        template<typename T>
        bool contains(Slice<T> slice, const typename decltype(slice)::Type& t)
        {
            return first(slice, t) != slice.end;
        }

        /* Split the slice from the first occurance of the element. */
        template<typename T>
        Split<T> before(Slice<T> slice, const typename decltype(slice)::Type& t)
        {
            return split(slice, first(slice, t));
        }

        /* Split the slice from the element on after the last occurance of the
         * element. */
        template<typename T>
        Split<T> after(Slice<T> slice, const typename decltype(slice)::Type& t)
        {
            return split(slice, last(slice, t) + 1);
        }
    } // namespace slice
} // namespace cthrice
