// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/scalar.cc"
#include "prelude/slice.cc"

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace cthrice
{
    /* Dynamic storage of contiguously placed elements. */
    template<typename Element>
    struct List : Slice<Element> {
        /* Pointer to the element one after the last allocated one. */
        Element* lst{nullptr};

        /* Amount of allocated element memory. */
        [[nodiscard]] constexpr ix capacity() const noexcept
        {
            ix const cap{lst - bgn};
            expect(cap >= 0, "Negative capacity!");
            return cap;
        }

        /* Amount of unused element memory. */
        [[nodiscard]] constexpr ix space() const noexcept
        {
            ix const spc{lst - end};
            expect(spc >= 0, "Negative space!");
            return spc;
        }

        /* Deallocate the memory. */
        constexpr void free() noexcept
        {
            std::free(bgn);
            bgn = end = lst = nullptr;
        }

        /* Remove all the elements. Keeps the memory. */
        constexpr void clear() noexcept
        {
            end = bgn;
        }

        /* Make sure the space is at least as big as the amount. Grows if
         * necessary by at least the current capacity. */
        constexpr void reserve(ix const amt) noexcept
        {
            expect(amt >= 0, "Reserving negative amount!");

            ix const grw{amt - space()};

            if (grw <= 0) {
                return;
            }

            ix const sze{size()};
            ix const cap{capacity()};

            ix const    nwc{cap + std::max(grw, cap)};
            auto* const mem{(Element*)std::realloc(bgn, sizeof(Element) * nwc)};
            expect(mem != nullptr, "List allocation failed!");

            bgn = mem;
            end = mem + sze;
            lst = mem + nwc;
        }

        /* Add the element to the end of the list. */
        constexpr void add(Element const& e) noexcept
        {
            reserve(1);
            *end++ = e;
        }

        /* Add the slice to the end of the list. */
        constexpr void add(Slice<const Element> const& slice) noexcept
        {
            ix const len{slice.size()};
            reserve(len);
            std::memmove((void*)end, (void*)slice.bgn, len);
            end += len;
        }

        /* Open space as big as the length at the index by moving the elements
         * and growing the memory if necessary. */
        constexpr void open(ix const i, ix const len) noexcept
        {
            reserve(len);
            Slice<Element> opened = slice(i, i + len);
            std::memmove((void*)opened.end, (void*)opened.bgn, len);
            end += len;
        }

        /* Put the element to the list at the index. */
        constexpr void put(ix const i, Element const& e) noexcept
        {
            open(i, 1);
            bgn[i] = e;
        }

        /* Put the slice to the list at the index. The slice should not point to
         * this list. */
        constexpr void put(ix const i, Slice<Element const> const& slice)
        {
            expect(
                !valid(slice.bgn) || !valid(slice.end),
                "Putting list to itself!");
            ix const len{slice.size()};
            open(i, len);
            std::memmove((void*)(bgn + i), (void*)slice.bgn, len);
        }
    };
} // namespace cthrice
