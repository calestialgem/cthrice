// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_BUFFER_HH
#define CTHRICE_BUFFER_HH 1

#include "error.hh"
#include "range.hh"

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace cthrice
{
    template<typename T>
    struct Buffer {
        T* bgn;
        T* end;
        T* lst;
    };

    /** Get the amount of elements. */
    template<typename T>
    size_t size(Buffer<T> bfr)
    {
        return bfr.end - bfr.bgn;
    }

    /** Get the amount of memory allocated in elements. */
    template<typename T>
    size_t capacity(Buffer<T> bfr)
    {
        return bfr.lst - bfr.bgn;
    }

    /** Get the amount of memory unused in elements. */
    template<typename T>
    size_t space(Buffer<T> bfr)
    {
        return bfr.lst - bfr.end;
    }

    /** Destroy a buffer. */
    template<typename T>
    [[nodiscard]] Buffer<T> destory(Buffer<T> bfr)
    {
        std::free(bfr.bgn);
        return {.bgn = nullptr, .end = nullptr, .lst = nullptr};
    }

    /** Clear a buffer. */
    template<typename T>
    [[nodiscard]] Buffer<T> clear(Buffer<T> bfr)
    {
        bfr.end = bfr.bgn;
        return bfr;
    }

    /** Get a immutable view of all the buffer. */
    template<typename T>
    Range<T> view(Buffer<T> bfr)
    {
        return {bfr.bgn, bfr.end};
    }

    /** Get a immutable view of a portion of the buffer at the end starting from
     * the given offset. */
    template<typename T>
    Range<T> view(Buffer<T> bfr, size_t off)
    {
        return {bfr.bgn + off, bfr.end};
    }

    /** Ensure that the buffer has at least the given amount of space. If it is
     * necessary, grows the buffer at least by the half of the capacity. */
    template<typename T>
    [[nodiscard]] Buffer<T> ensure_space(Buffer<T> bfr, size_t spc)
    {
        size_t curspc = space(bfr);
        if (spc <= curspc) {
            return bfr;
        }

        // Find the growth amount, which should be at least the half of the
        // current capacity.
        size_t grw = spc - curspc;
        size_t sze = size(bfr);
        size_t cap = capacity(bfr);
        size_t nwc = std::max(grw, cap / 2) + cap;

        T* mem = (T*)std::realloc(bfr.bgn, nwc);
        cthrice_check(mem == nullptr, "Could not allocate buffer!");

        return {.bgn = mem, .end = mem + sze, .lst = mem + nwc};
    }

    /** Put an element to the buffer. Grows the buffer if necessary. */
    template<typename T>
    [[nodiscard]] Buffer<T> put(Buffer<T> bfr, T elmt)
    {
        bfr          = ensure_space(bfr, 1);
        *(bfr.end++) = elmt;
        return bfr;
    }

    /** Put a view's contents to the buffer. Grows the buffer if necessary. */
    template<typename T>
    [[nodiscard]] Buffer<T> put(Buffer<T> bfr, Range<T> view)
    {
        size_t n = size(view);
        bfr      = ensure_space(bfr, n);
        std::memcpy(bfr.end, view.bgn, n * sizeof(T));
        bfr.end += n;
        return bfr;
    }
} // namespace cthrice

#endif // CTHRICE_BUFFER_HH
