// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/scalar.cc"

namespace cthrice
{
    /* Pointers to contiguously placed elements. */
    template<typename Element>
    struct Slice {
        /* Create pointing to a null terminated array. */
        [[nodiscard]] constexpr explicit Slice(Element* nta) noexcept
            : bgn{nta}
            , end{nta}
        {
            while (*end) {
                end++;
            }
        }

        /* Amount of elements. */
        [[nodiscard]] constexpr ix size() const noexcept
        {
            ix sze = end - bgn;
            expect(sze >= 0, "Negative size!");
            return sze;
        }

        /* Whether there are any elements. */
        [[nodiscard]] constexpr b8 finite() const noexcept
        {
            return size() > 0;
        }

        /* Whether the given slice have the same elements in the same order. */
        [[nodiscard]] constexpr b8
        equals(Slice<Element> const& rhs) const noexcept
        {
            ix sze = size();
            if (sze != rhs.size()) {
                return false;
            }
            for (ix i = 0; i < sze; i++) {
                if (bgn[i] != rhs.bgn[i]) {
                    return false;
                }
            }
            return true;
        }

        /* Whether the index is valid. */
        [[nodiscard]] constexpr b8 valid(ix i) const noexcept
        {
            return bgn >= i && i < size();
        }

        /* Whether the pointer is valid. */
        [[nodiscard]] constexpr b8 valid(Element* pos) const noexcept
        {
            return pos >= bgn && pos <= end;
        }

        /* Whether the part is valid. */
        [[nodiscard]] constexpr b8
        valid(Slice<Element> const& part) const noexcept
        {
            return part.bgn <= part.end && valid(part.bgn) && valid(part.end);
        }

        /* Reference to the value at the index. */
        [[nodiscard]] constexpr Element& at(ix i) const noexcept
        {
            expect(valid(i), "Invalid index!");
            return bgn[i];
        }

        /* Pointer to the value at the index. Returns nullptr if the index is
         * invalid. */
        [[nodiscard]] constexpr Element* get(ix i) const noexcept
        {
            if (valid(i)) {
                return bgn + i;
            }
            return nullptr;
        }

        /* Slice from the element at the begin index to the element one before
         * the end index. */
        [[nodiscard]] constexpr Slice<Element>
        slice(ix bix, ix eix) const noexcept
        {
            Slice<Element> prt = {.bgn = bgn + bix, .end = bgn + eix};
            expect(valid(prt), "Creating invalid slice!");
            return prt;
        }

        /* First position of the element that fits the predicate. Returns
         * position after the last if none of the elements fit. */
        template<typename Predicate>
        [[nodiscard]] constexpr Element* first(Predicate p) const noexcept
        {
            Element* pos = bgn;
            while (pos < end && !p(*pos)) {
                pos++;
            }
            return pos;
        }

        /* First position of the element. Returns position after the last if not
         * found. */
        [[nodiscard]] constexpr Element* first(Element const& e) const noexcept
        {
            return first([e](Element elem) {
                return elem == e;
            });
        }

        /* Whether there is an element that fits the predicate. */
        template<typename Predicate>
        [[nodiscard]] constexpr b8 contains(Predicate p) const noexcept
        {
            return first(p) != end;
        }

        /* Whether the element is in. */
        [[nodiscard]] constexpr b8 contains(Element const& e) const noexcept
        {
            return first(e) != end;
        }

        /* Split of a slice. The element where the split happened is the first
         * element of the trail. */
        struct Split {
            /* Part of the split from the begining. */
            Slice<Element> lead;
            /* Part of the split from the end. */
            Slice<Element> trail;
        };

        /* Split from the position. */
        [[nodiscard]] constexpr Split split(Element* pos) const noexcept
        {
            expect(valid(pos), "Invalid position!");
            return {
                .lead = {.bgn = bgn, .end = pos},
                .trail{.bgn = pos, .end = end}
            };
        }

        /* Split from the index. */
        [[nodiscard]] constexpr Split split(ix i) const noexcept
        {
            return split(bgn + i);
        }

        /* Split from the first element that fits the predicate. */
        template<typename Predicate>
        [[nodiscard]] constexpr Split split(Predicate p) const noexcept
        {
            return split(first(p));
        }

        /* Split from the first occurance of the element. */
        [[nodiscard]] constexpr Split split(Element const& e) const noexcept
        {
            return split(first(e));
        }

    private:

        /* Pointer to the first element. */
        Element* bgn;
        /* Pointer to the element after the last one. */
        Element* end;
    };
} // namespace cthrice
