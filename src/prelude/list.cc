// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "types.cc"
#include "view.cc"

#include <cstdlib>
#include <cstring>

namespace cthrice
{
    /* Mutable and dynamic storage of contiguously placed elements. */
    template<typename T>
    struct List {
        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the end. */
        T* end;
        /* Pointer to the element one after the last one. */
        T* lst;

        /* Amount of elements. */
        static ix size(List<T> list)
        {
            ix sze = list.end - list.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Amount of allocated element memory. */
        static ix capacity(List<T> list)
        {
            ix cap = list.lst - list.bgn;
            debug(cap >= 0, "Negative capacity!");
            return cap;
        }

        /* Amount of unused element memory. */
        static ix space(List<T> list)
        {
            ix spc = list.lst - list.end;
            debug(spc >= 0, "Negative space!");
            return spc;
        }

        /* Whether the view is valid. */
        static bool view_valid(List<T> list, View<T> view)
        {
            return view.bgn >= list.bgn && view.end <= list.end;
        }

        /* Immutable view of a part of the list from the element at the begin
         * index to the element one before the end index. */
        static View<T> view_part(List<T> list, ix bix, ix eix)
        {
            View res = {.bgn = list.bgn + bix, .end = list.bgn + eix};
            debug(view_valid(res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the end of the list from the element at the begin
         * index to the last element. */
        static View<T> view_end(List<T> list, ix bix)
        {
            View<T> res = {.bgn = list.bgn + bix, .end = list.end};
            debug(view_valid(list, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the list from the first element to the last
         * element. */
        static View<T> view(List<T> list)
        {
            return {.bgn = list.bgn, .end = list.end};
        }

        /* Deallocate the memory. */
        [[nodiscard]] static List<T> free(List<T> list)
        {
            std::free(list.bgn);
            list.bgn = list.end = list.lst = nullptr;
            return list;
        }

        /* Grow the capacity to the next powers of 16 until the space that is
         * wanted exists in the list. */
        [[nodiscard]] static List<T> ensure_space(List<T> list, ix wsp)
        {
            debug(wsp >= 0, "Wanted space is negative!");

            if (space(list) >= wsp) {
                return list;
            }

            ix sze = size(list);
            ix wcp = sze + wsp;

            // Find the new capacity.
            ix nwc = capacity(list);
            if (nwc == 0) {
                nwc = 1;
            }
            while (nwc < wcp) {
                nwc <<= 4;
            }
            debug(nwc > 0, "Allocating nonpositive capacity!");

            T* mem = (T*)std::realloc(list.bgn, sizeof(T) * nwc);
            check(mem != nullptr, "Could not allocate!");

            list.bgn = mem;
            list.end = mem + sze;
            list.lst = mem + nwc;
            return list;
        }

        /* Add the element to the end of the list. */
        [[nodiscard]] static List<T> add(List<T> list, T t)
        {
            list        = ensure_space(list, 1);
            *list.end++ = t;
            return list;
        }

        /* Add the view to the end of the list. */
        [[nodiscard]] static List<T> add_view(List<T> list, View<T> view)
        {
            ix len = View<T>::size(view);
            list   = ensure_space(list, len);
            std::memmove(list.end, view.bgn, len);
            list.end += len;
            return list;
        }

        /* Open space at and after the given index by moving elements and
         * growing the memory if necessary. */
        [[nodiscard]] static List<T> open_space(List<T> list, ix i, ix len)
        {
            list        = ensure_space(list, len);
            View opened = view_part(list, i, i + len);
            std::memmove(opened.end, opened.bgn, len);
            list.end += len;
            return list;
        }

        /* Put the element to the list at the index. */
        [[nodiscard]] static List<T> put(List<T> list, T t, ix i)
        {
            list        = open_space(list, i, 1);
            list.bgn[i] = t;
            return list;
        }

        /* Put a view to the list at the index. */
        [[nodiscard]] static List<T> put_view(List<T> list, View<T> view, ix i)
        {
            ix len = View<T>::size(view);
            list   = open_space(list, i, len);
            std::memmove(list.bgn + i, view.bgn, len);
            return list;
        }
    };
} // namespace cthrice
