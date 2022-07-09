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
     * allocated memory is same as the used memory. */
    template<typename T>
    struct Array {
        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the end. */
        T* end;
    };

    namespace array
    {
        /* Amount of elements. */
        template<typename T>
        Ix size(Array<T> array)
        {
            Ix sze = array.end - array.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether there are any elements. */
        template<typename T>
        bool finite(Array<T> array)
        {
            return size(array) > 0;
        }

        /* Whether the index is valid. */
        template<typename T>
        bool valid(Array<T> array, Ix ix)
        {
            return ix >= 0 && ix < size(array);
        }

        /* Whether the view is valid. */
        template<typename T>
        bool valid_view(Array<T> array, View<T> view)
        {
            return view.bgn >= array.bgn && view.end <= array.end;
        }

        /* Reference to the value at the index. */
        template<typename T>
        T& at(Array<T> array, Ix ix)
        {
            debug(valid(array, ix), "Invalid index!");
            return array.bgn[ix];
        }

        /* Pointer to the value at the index. Returns null if the index is
         * invalid. */
        template<typename T>
        T* get(Array<T> array, Ix ix)
        {
            if (valid(array, ix)) {
                return array.bgn + ix;
            }
            return nullptr;
        }

        /* Immutable view of a part of the array from the element at the begin
         * index to the element one before the end index. */
        template<typename T>
        View<T> view_part(Array<T> array, Ix bix, Ix eix)
        {
            View<T> res = {.bgn = array.bgn + bix, .end = array.bgn + eix};
            debug(valid_view(array, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the end of the array from the element at the begin
         * index to the last element. */
        template<typename T>
        View<T> view_end(Array<T> array, Ix bix)
        {
            View<T> res = {.bgn = array.bgn + bix, .end = array.end};
            debug(valid_view(array, res), "Creating invalid view!");
            return res;
        }

        /* Immutable view of the array from the first element to the last
         * element. */
        template<typename T>
        View<T> view(Array<T> array)
        {
            return {.bgn = array.bgn, .end = array.end};
        }

        /* Deallocate the memory. */
        template<typename T>
        [[nodiscard]] Array<T> free(Array<T> array)
        {
            std::free(array.bgn);
            array.bgn = array.end = nullptr;
            return array;
        }

        /* Grow by the amount. */
        template<typename T>
        [[nodiscard]] Array<T> grow(Array<T> array, Ix amt)
        {
            debug(amt >= 0, "Growing by negative amount!");

            Ix nws = size(array) + amt;
            T* mem = (T*)std::realloc(array.bgn, sizeof(T) * nws);
            check(mem != nullptr, "Could not allocate!");

            array.bgn = mem;
            array.end = mem + nws;
            return array;
        }

        /* Add the element to the end of the array. */
        template<typename T>
        [[nodiscard]] Array<T> add(Array<T> array, T t)
        {
            array         = grow(array, 1);
            array.end[-1] = t;
            return array;
        }

        /* Add the view to the end of the array. */
        template<typename T>
        [[nodiscard]] Array<T> add_view(Array<T> array, View<T> view)
        {
            Ix len = view::size(view);
            array  = grow(array, len);
            std::memmove((void*)(array.end - len), (void*)view.bgn, len);
            return array;
        }

        /* Open space at the index by moving the elements and growing the
         * memory. */
        template<typename T>
        [[nodiscard]] Array<T> open(Array<T> array, Ix i, Ix len)
        {
            array          = grow(array, len);
            View<T> opened = view_part(array, i, i + len);
            std::memmove((void*)opened.end, (void*)opened.bgn, len);
            return array;
        }

        /* Put the element to the array at the index. */
        template<typename T>
        [[nodiscard]] Array<T> put(Array<T> array, Ix i, T t)
        {
            array        = open(array, i, 1);
            array.bgn[i] = t;
            return array;
        }

        /* Put the view to the array at the index. */
        template<typename T>
        [[nodiscard]] Array<T> put_view(Array<T> array, Ix i, View<T> view)
        {
            Ix len = view::size(view);
            array  = open(array, i, len);
            std::memmove((void*)(array.bgn + i), (void*)view.bgn, len);
            return array;
        }
    } // namespace array
} // namespace cthrice
