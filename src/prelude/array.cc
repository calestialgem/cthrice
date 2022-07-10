// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "slice.cc"
#include "types.cc"

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

        /* Whether the slice is valid. */
        template<typename T>
        bool valid_slice(Array<T> array, Slice<const T> slice)
        {
            return slice.bgn >= array.bgn && slice.end <= array.end;
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

        /* Immutable slice of a part of the array from the element at the begin
         * index to the element one before the end index. */
        template<typename T>
        Slice<const T> slice_part(Array<T> array, Ix bix, Ix eix)
        {
            Slice<const T> res = {
                .bgn = array.bgn + bix,
                .end = array.bgn + eix};
            debug(valid_slice(array, res), "Creating invalid slice!");
            return res;
        }

        /* Immutable slice of the end of the array from the element at the begin
         * index to the last element. */
        template<typename T>
        Slice<const T> slice_end(Array<T> array, Ix bix)
        {
            Slice<const T> res = {.bgn = array.bgn + bix, .end = array.end};
            debug(valid_slice(array, res), "Creating invalid slice!");
            return res;
        }

        /* Immutable slice of the array from the first element to the last
         * element. */
        template<typename T>
        Slice<const T> slice(Array<T> array)
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

        /* Add the slice to the end of the array. */
        template<typename T>
        [[nodiscard]] Array<T> add_slice(Array<T> array, Slice<const T> slice)
        {
            Ix len = slice::size(slice);
            array  = grow(array, len);
            std::memmove((void*)(array.end - len), (void*)slice.bgn, len);
            return array;
        }

        /* Open space at the index by moving the elements and growing the
         * memory. */
        template<typename T>
        [[nodiscard]] Array<T> open(Array<T> array, Ix i, Ix len)
        {
            array                 = grow(array, len);
            Slice<const T> opened = slice_part(array, i, i + len);
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

        /* Put the slice to the array at the index. */
        template<typename T>
        [[nodiscard]] Array<T>
        put_slice(Array<T> array, Ix i, Slice<const T> slice)
        {
            Ix len = slice::size(slice);
            array  = open(array, i, len);
            std::memmove((void*)(array.bgn + i), (void*)slice.bgn, len);
            return array;
        }
    } // namespace array
} // namespace cthrice
