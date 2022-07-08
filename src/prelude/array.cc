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
    /* Mutable and dynamic storage of contiguously placed elements where all
     * elements are valid and all memory is used. */
    template<typename T>
    struct Array {
        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the end. */
        T* end;

        /* Amount of elements. */
        static ix size(Array<T> array)
        {
            ix sze = array.end - array.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether the view is valid. */
        static bool view_valid(Array<T> array, View<T> view)
        {
            return view.bgn >= array.bgn && view.end <= array.end;
        }

        /* Immutable view of a part of the array from the element at the begin
         * index to the element one before the end index. */
        static View<T> view_part(Array<T> array, ix bix, ix eix)
        {
            View<T> res = {.bgn = array.bgn + bix, .end = array.bgn + eix};
            debug(view_valid(array, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the end of the array from the element at the begin
         * index to the last element. */
        static View<T> view_end(Array<T> array, ix bix)
        {
            View<T> res = {.bgn = array.bgn + bix, .end = array.end};
            debug(view_valid(array, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the array from the first element to the last
         * element. */
        static View<T> view(Array<T> array)
        {
            return {.bgn = array.bgn, .end = array.end};
        }

        /* Deallocate the memory. */
        [[nodiscard]] static Array<T> free(Array<T> array)
        {
            std::free(array.bgn);
            array.bgn = array.end = nullptr;
            return array;
        }

        /* Grow by the amount. */
        [[nodiscard]] static Array<T> grow(Array<T> array, ix amt)
        {
            debug(amt >= 0, "Growing by negative amount!");

            ix nws = size(array) + amt;
            T* mem = (T*)std::realloc(array.bgn, sizeof(T) * nws);
            check(mem != nullptr, "Could not allocate!");

            array.bgn = mem;
            array.end = mem + nws;
            return array;
        }

        /* Add the element to the end of the array. */
        [[nodiscard]] static Array<T> add(Array<T> array, T t)
        {
            ix pos         = size(array);
            array          = grow(array, 1);
            array.bgn[pos] = t;
            return array;
        }

        /* Add the view to the end of the array. */
        [[nodiscard]] static Array<T> add_view(Array<T> array, View<T> view)
        {
            ix pos = size(array);
            ix len = View<T>::size(view);
            array  = grow(array, len);
            std::memmove((void*)(array.bgn + pos), (void*)view.bgn, len);
            return array;
        }

        /* Open space at the index by moving elements and growing. */
        [[nodiscard]] static Array<T> open(Array<T> array, ix i, ix len)
        {
            array          = grow(array, len);
            View<T> opened = view_part(array, i, i + len);
            std::memmove((void*)opened.end, (void*)opened.bgn, len);
            return array;
        }

        /* Put the element to the array at the index. */
        [[nodiscard]] static Array<T> put(Array<T> array, ix i, T t)
        {
            array        = open(array, i, 1);
            array.bgn[i] = t;
            return array;
        }

        /* Put a view to the array at the index. */
        [[nodiscard]] static Array<T>
        put_view(Array<T> array, ix i, View<T> view)
        {
            ix len = View<T>::size(view);
            array  = open(array, i, len);
            std::memmove((void*)(array.bgn + i), (void*)view.bgn, len);
            return array;
        }
    };
} // namespace cthrice
