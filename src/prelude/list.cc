// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/scalar.cc"
#include "prelude/view.cc"

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace cthrice
{
/* Dynamic storage of contiguous elements. */
template<typename Element>
struct List {
    /* Amount of elements. */
    [[nodiscard]] constexpr ix size() const noexcept
    {
        return last - first;
    }

    /* Amount of allocated elements. */
    [[nodiscard]] constexpr ix capacity() const noexcept
    {
        return lastAllocated - first;
    }

    /* Amount of unused elements. */
    [[nodiscard]] constexpr ix space() const noexcept
    {
        return lastAllocated - last;
    }

    /* Pointer to the value at the index. Returns nullptr if the index is
     * invalid. */
    [[nodiscard]] constexpr Element* get(ix index) const noexcept
    {
        if (index >= 0 && index < size()) {
            return first + index;
        }
        return nullptr;
    }

    /* Reference to the element at the index. */
    [[nodiscard]] constexpr Element& at(ix index) const noexcept
    {
        Element* pointer{get(index)};
        expect(pointer != nullptr, "Index out of bounds!");
        return *pointer;
    }

    /* Deallocate the memory. */
    constexpr void free() noexcept
    {
        std::free(first);
        first         = nullptr;
        last          = nullptr;
        lastAllocated = nullptr;
    }

    /* Remove all the elements. Keeps the memory. */
    constexpr void clear() noexcept
    {
        last = first;
    }

    /* Make sure the space is at least as big as the amount. Grows if necessary
     * by at least the half of the current capacity. */
    constexpr void reserve(ix amount) noexcept
    {
        expect(amount >= 0, "Reserving negative amount!");
        ix growth{amount - space()};
        if (growth <= 0) {
            return;
        }

        ix size{this->size()};
        ix capacity{this->capacity()};
        ix newCapacity{capacity + std::max(growth, capacity >> 1)};

        Element* memory{
            (Element*)std::realloc(first, sizeof(Element) * newCapacity)};
        expect(memory != nullptr, "Could not allocate!");

        first         = memory;
        last          = memory + size;
        lastAllocated = memory + newCapacity;
    }

    /* Add the element to the end. */
    constexpr void add(Element const& element) noexcept
    {
        reserve(1);
        *last++ = element;
    }

    /* Add the view to the end. */
    constexpr void add_view(View<Element> const& view) noexcept
    {
        ix size{view.size()};
        reserve(size);
        std::memmove((void*)last, (void const*)view.get(0), size);
        last += size;
    }

    /* Open space at the index by moving elements and growing the memory if
     * necessary. */
    constexpr void open(ix index, ix amount) noexcept
    {
        reserve(amount);
        std::memmove(
            (void*)(first + index),
            (void const*)(first + index + amount),
            amount);
        last += amount;
    }

    /* Put the element to the index. */
    constexpr void put(ix index, Element const& element) noexcept
    {
        open(index, 1);
        first[index] = element;
    }

    /* Put the view to the index. */
    constexpr void put_view(ix index, View<Element> const& view) noexcept
    {
        ix size{view.size()};
        open(index, size);
        std::memmove((void*)(first + index), (void const*)view.get(0), size);
    }

    /* Call the consumer for all the elements in the view. */
    template<Function<void, Element> Consumer>
    constexpr void consume(Consumer consumer) const noexcept
    {
        for (Element const* i{first}; i < last; i++) {
            consumer(*i);
        }
    }

    /* Empty list. */
    [[nodiscard]] constexpr List() noexcept = default;

private:

    /* Border before the first element. */
    Element* first{nullptr};
    /* Border after the last element. */
    Element* last{nullptr};
    /* Border after the last allocated element. */
    Element* lastAllocated{nullptr};
};
} // namespace cthrice
