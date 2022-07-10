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
     * allocated memory is bigger than the used memory. */
    template<typename T>
    struct List {
        /* Pointer to the element at the begining. */
        T* bgn;
        /* Pointer to the element one after the last valid one. */
        T* end;
        /* Pointer to the element one after the last allocated one. */
        T* lst;
    };

    namespace list
    {
        /* Amount of elements. */
        template<typename T>
        Ix size(List<T> list)
        {
            Ix sze = list.end - list.bgn;
            debug(sze >= 0, "Negative size!");
            return sze;
        }

        /* Amount of allocated element memory. */
        template<typename T>
        Ix capacity(List<T> list)
        {
            Ix cap = list.lst - list.bgn;
            debug(cap >= 0, "Negative capacity!");
            return cap;
        }

        /* Amount of unused element memory. */
        template<typename T>
        Ix space(List<T> list)
        {
            Ix spc = list.lst - list.end;
            debug(spc >= 0, "Negative space!");
            return spc;
        }

        /* Whether there are any elements. */
        template<typename T>
        bool finite(List<T> list)
        {
            return size(list) > 0;
        }

        /* Whether the index is valid. */
        template<typename T>
        bool valid(List<T> list, Ix ix)
        {
            return ix >= 0 && ix < size(list);
        }

        /* Whether the slice is valid. */
        template<typename T>
        bool valid_slice(List<T> list, Slice<const T> slice)
        {
            return slice.bgn >= list.bgn && slice.end <= list.end;
        }

        /* Reference to the value at the index. */
        template<typename T>
        T& at(List<T> list, Ix ix)
        {
            debug(valid(list, ix), "Invalid index!");
            return list.bgn[ix];
        }

        /* Pointer to the value at the index. Returns null if the index is
         * invalid. */
        template<typename T>
        T* get(List<T> list, Ix ix)
        {
            if (valid(list, ix)) {
                return list.bgn + ix;
            }
            return nullptr;
        }

        /* Immutable slice of a part of the list from the element at the begin
         * index to the element one before the end index. */
        template<typename T>
        Slice<const T> slice_part(List<T> list, Ix bix, Ix eix)
        {
            Slice<const T> res = {.bgn = list.bgn + bix, .end = list.bgn + eix};
            debug(valid_slice(list, res), "Creating invalid slice!");
            return res;
        }

        /* Immutable slice of the end of the list from the element at the begin
         * index to the last valid element. */
        template<typename T>
        Slice<const T> slice_end(List<T> list, Ix bix)
        {
            Slice<const T> res = {.bgn = list.bgn + bix, .end = list.end};
            debug(valid_slice(list, res), "Creating invalid slice!");
            return res;
        }

        /* Immutable slice of the list from the first element to the last
         * valid element. */
        template<typename T>
        Slice<const T> slice(List<T> list)
        {
            return {.bgn = list.bgn, .end = list.end};
        }

        /* Deallocate the memory. */
        template<typename T>
        [[nodiscard]] List<T> free(List<T> list)
        {
            std::free(list.bgn);
            list.bgn = list.end = list.lst = nullptr;
            return list;
        }

        /* Remove all the elements. Keeps the memory. */
        template<typename T>
        [[nodiscard]] List<T> clear(List<T> list)
        {
            list.end = list.bgn;
            return list;
        }

        /* Make sure the space is at least as big as the amount. Grows if
         * necessary by at least the half of the current capacity. */
        template<typename T>
        [[nodiscard]] List<T> reserve(List<T> list, Ix amt)
        {
            debug(amt >= 0, "Reserving negative amount!");

            Ix grw = amt - space(list);

            if (grw <= 0) {
                return list;
            }

            Ix sze = size(list);
            Ix cap = capacity(list);

            Ix nwc = cap + std::max(grw, cap >> 1);
            T* mem = (T*)std::realloc(list.bgn, sizeof(T) * nwc);
            check(mem != nullptr, "Could not allocate!");

            list.bgn = mem;
            list.end = mem + sze;
            list.lst = mem + nwc;
            return list;
        }

        /* Add the element to the end of the list. */
        template<typename T>
        [[nodiscard]] List<T> add(List<T> list, T t)
        {
            list        = reserve(list, 1);
            *list.end++ = t;
            return list;
        }

        /* Add the slice to the end of the list. */
        template<typename T>
        [[nodiscard]] List<T> add_slice(List<T> list, Slice<const T> slice)
        {
            Ix len = slice::size(slice);
            list   = reserve(list, len);
            std::memmove((void*)list.end, (void*)slice.bgn, len);
            list.end += len;
            return list;
        }

        /* Open space at the index by moving the elements and growing the memory
         * if necessary. */
        template<typename T>
        [[nodiscard]] List<T> open(List<T> list, Ix i, Ix len)
        {
            list                  = reserve(list, len);
            Slice<const T> opened = slice_part(list, i, i + len);
            std::memmove((void*)opened.end, (void*)opened.bgn, len);
            list.end += len;
            return list;
        }

        /* Put the element to the list at the index. */
        template<typename T>
        [[nodiscard]] List<T> put(List<T> list, Ix i, T t)
        {
            list        = open(list, i, 1);
            list.bgn[i] = t;
            return list;
        }

        /* Put the slice to the list at the index. */
        template<typename T>
        [[nodiscard]] List<T>
        put_slice(List<T> list, Ix i, Slice<const T> slice)
        {
            Ix len = slice::size(slice);
            list   = open(list, i, len);
            std::memmove((void*)(list.bgn + i), (void*)slice.bgn, len);
            return list;
        }
    } // namespace list
} // namespace cthrice
