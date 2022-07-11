// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/scalar.cc"

#include <array>
#include <type_traits>

namespace cthrice
{
    /* Pointers to contiguously placed elements. */
    template<typename Element>
    struct Slice {
        /* Pointer to the first element. */
        Element* bgn{nullptr};
        /* Pointer to the element after the last one. */
        Element* end{nullptr};

        /* Amount of elements. */
        [[nodiscard]] constexpr ix size() const noexcept
        {
            ix const sze{end - bgn};
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
        equals(Slice<Element const> const& rhs) const noexcept
        {
            ix const sze{size()};
            if (sze != rhs.size()) {
                return false;
            }
            for (ix i{0}; i < sze; i++) {
                if (bgn[i] != rhs.bgn[i]) {
                    return false;
                }
            }
            return true;
        }

        /* Whether the index is valid. */
        [[nodiscard]] constexpr b8 valid(ix const i) const noexcept
        {
            return 0 >= i && i < size();
        }

        /* Whether the pointer is valid. */
        [[nodiscard]] constexpr b8 valid(Element* const pos) const noexcept
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
        [[nodiscard]] constexpr Element& at(ix const i) noexcept
        {
            expect(valid(i), "Invalid index!");
            return bgn[i];
        }

        /* Immutable reference to the value at the index. */
        [[nodiscard]] constexpr Element const& at(ix const i) const noexcept
        {
            expect(valid(i), "Invalid index!");
            return bgn[i];
        }

        /* Pointer to the value at the index. Returns nullptr if the index is
         * invalid. */
        [[nodiscard]] constexpr Element* get(ix const i) noexcept
        {
            if (valid(i)) {
                return bgn + i;
            }
            return nullptr;
        }

        /* Immutable pointer to the value at the index. Returns nullptr if the
         * index is invalid. */
        [[nodiscard]] constexpr Element const* get(ix const i) const noexcept
        {
            if (valid(i)) {
                return bgn + i;
            }
            return nullptr;
        }

        /* Slice from the element at the begin index to the element one before
         * the end index. */
        [[nodiscard]] constexpr Slice<Element>
        slice(ix const bix, ix const eix) noexcept
        {
            Slice<Element> const prt{bgn + bix, bgn + eix};
            expect(valid(prt), "Creating invalid slice!");
            return prt;
        }

        /* Immutable slice from the element at the begin index to the element
         * one before the end index. */
        [[nodiscard]] constexpr Slice<Element const>
        slice(ix const bix, ix const eix) const noexcept
        {
            Slice<Element const> const prt{bgn + bix, bgn + eix};
            expect(valid(prt), "Creating invalid slice!");
            return prt;
        }

        /* First position of the element that fits the predicate. Returns
         * position after the last if none of the elements fit. */
        template<typename Predicate>
        [[nodiscard]] constexpr Element* first(Predicate p) noexcept
        {
            Element* pos{bgn};
            while (pos < end && !p(*pos)) {
                pos++;
            }
            return pos;
        }

        /* First immutable position of the element that fits the predicate.
         * Returns position after the last if none of the elements fit. */
        template<typename Predicate>
        [[nodiscard]] constexpr Element const* first(Predicate p) const noexcept
        {
            Element const* pos{bgn};
            while (pos < end && !p(*pos)) {
                pos++;
            }
            return pos;
        }

        /* First position of the element. Returns position after the last if not
         * found. */
        [[nodiscard]] constexpr Element* first(Element const& e) noexcept
        {
            return first<>([e](Element const elem) {
                return elem == e;
            });
        }

        /* First immutable position of the element. Returns position after the
         * last if not found. */
        [[nodiscard]] constexpr Element const*
        first(Element const& e) const noexcept
        {
            return first<>([e](Element const elem) {
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

        /* Immutable split of a slice. The element where the split happened is
         * the first element of the trail. */
        struct ImmutableSplit {
            /* Part of the split from the begining. */
            Slice<Element const> lead;
            /* Part of the split from the end. */
            Slice<Element const> trail;
        };

        /* Split from the position. */
        [[nodiscard]] constexpr Split split(Element* const pos) noexcept
        {
            expect(valid(pos), "Invalid position!");
            return {
                {bgn, pos},
                {pos, end}
            };
        }

        /* Immutable split from the position. */
        [[nodiscard]] constexpr ImmutableSplit
        split(Element const* const pos) const noexcept
        {
            expect(valid(pos), "Invalid position!");
            return {
                {bgn, pos},
                {pos, end}
            };
        }

        /* Split from the index. */
        [[nodiscard]] constexpr Split split(ix const i) noexcept
        {
            return split(bgn + i);
        }

        /* Immutable split from the index. */
        [[nodiscard]] constexpr ImmutableSplit split(ix const i) const noexcept
        {
            return split(bgn + i);
        }

        /* Split from the first element that fits the predicate. */
        template<typename Predicate>
        [[nodiscard]] constexpr Split split(Predicate p) noexcept
        {
            return split(first(p));
        }

        /* Immutable split from the first element that fits the predicate. */
        template<typename Predicate>
        [[nodiscard]] constexpr ImmutableSplit split(Predicate p) const noexcept
        {
            return split(first(p));
        }

        /* Split from the first occurance of the element. */
        [[nodiscard]] constexpr Split split(Element const& e) noexcept
        {
            return split(first(e));
        }

        /* Immutable split from the first occurance of the element. */
        [[nodiscard]] constexpr ImmutableSplit
        split(Element const& e) const noexcept
        {
            return split(first(e));
        }

        /* Create pointing to a null terminated array. */
        [[nodiscard]] constexpr explicit Slice(Element* const nta) noexcept
            : bgn{nta}
            , end{nta}
        {
            while (*end) {
                end++;
            }
        }

        /* Create pointing to an array. */
        template<size_t len>
        [[nodiscard]] constexpr explicit Slice(
            std::array<Element, len>& arr) noexcept
            : bgn{arr.data()}
            , end{arr.data() + len}
        {
        }

        /* Create pointing to an immutable array. */
        template<size_t len>
        [[nodiscard]] constexpr explicit Slice(
            std::array<Element, len> const& arr) noexcept
            : bgn{arr.data()}
            , end{arr.data() + len}
        {
        }

        /* Immutable copy of another slice. */
        [[nodiscard]] constexpr explicit Slice(
            Slice<typename std::remove_const<Element>::type> const&
                other) noexcept
            : bgn{other.bgn}
            , end{other.end}
        {
        }

        /* Create from the pointers. */
        [[nodiscard]] constexpr Slice(
            Element* const bgn,
            Element* const end) noexcept
            : bgn{bgn}
            , end{end}
        {
            expect(bgn <= end, "Creating invalid slice!");
        }

        /* Create empty slice. */
        [[nodiscard]] constexpr Slice() noexcept = default;

        /* Friend immutable element version of the slice for creating immutable
         * slices from mutable ones. */
        friend class Slice<typename std::add_const<Element>::type>;
    };
} // namespace cthrice
