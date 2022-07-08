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
    /* Mutable and dynamic storage of contiguously placed elements where
     * allocated memory is bigger than the used memory. */
    template<typename T>
    struct List {
        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the last valid one. */
        T* end;
        /* Pointer to the element one after the last allocated one. */
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
            View<T> res = {.bgn = list.bgn + bix, .end = list.bgn + eix};
            debug(view_valid(list, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the end of the list from the element at the begin
         * index to the last valid element. */
        static View<T> view_end(List<T> list, ix bix)
        {
            View<T> res = {.bgn = list.bgn + bix, .end = list.end};
            debug(view_valid(list, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the list from the first element to the last
         * valid element. */
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

        /* Remove all the elements. Keeps the memory. */
        [[nodiscard]] static List<T> clean(List<T> list)
        {
            list.end = list.bgn;
            return list;
        }

        /* Make sure the space is at least as big as the amount. Grows if
         * necessary by at least the half of the current capacity. */
        [[nodiscard]] static List<T> reserve(List<T> list, ix amt)
        {
            debug(amt >= 0, "Reserving negative amount!");

            ix grw = amt - space(list);

            if (grw <= 0) {
                return list;
            }

            ix sze = size(list);
            ix cap = capacity(list);

            ix nwc = cap + std::max(grw, cap >> 1);
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
            list        = reserve(list, 1);
            *list.end++ = t;
            return list;
        }

        /* Add the view to the end of the list. */
        [[nodiscard]] static List<T> add_view(List<T> list, View<T> view)
        {
            ix len = View<T>::size(view);
            list   = reserve(list, len);
            std::memmove((void*)list.end, (void*)view.bgn, len);
            list.end += len;
            return list;
        }

        /* Open space at the index by moving the elements and growing the memory
         * if necessary. */
        [[nodiscard]] static List<T> open(List<T> list, ix i, ix len)
        {
            list           = reserve(list, len);
            View<T> opened = view_part(list, i, i + len);
            std::memmove((void*)opened.end, (void*)opened.bgn, len);
            list.end += len;
            return list;
        }

        /* Put the element to the list at the index. */
        [[nodiscard]] static List<T> put(List<T> list, ix i, T t)
        {
            list        = open(list, i, 1);
            list.bgn[i] = t;
            return list;
        }

        /* Put the view to the list at the index. */
        [[nodiscard]] static List<T> put_view(List<T> list, ix i, View<T> view)
        {
            ix len = View<T>::size(view);
            list   = open(list, i, len);
            std::memmove((void*)(list.bgn + i), (void*)view.bgn, len);
            return list;
        }
    };
} // namespace cthrice
