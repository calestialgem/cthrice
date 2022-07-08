// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "types.cc"
#include "view.cc"

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
            ix cap = list.end - list.bgn;
            debug(cap >= 0, "Negative capacity!");
            return cap;
        }

        /* Amount of unused element memory. */
        static ix space(List<T> list)
        {
            ix spc = list.end - list.bgn;
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
        static View<T> view_part(List<T> list, ix bof, ix eof)
        {
            View res = {.bgn = list.bgn + bof, .end = list.bgn + eof};
            debug(view_valid(res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the end of the list from the element at the begin
         * index to the last element. */
        static View<T> view_end(List<T> list, ix bof)
        {
            View res = {.bgn = list.bgn + bof, .end = list.end};
            debug(view_valid(res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the list from the first element to the last
         * element. */
        static View<T> view(List<T> list)
        {
            return {.bgn = list.bgn, .end = list.end};
        }
    };
} // namespace cthrice
