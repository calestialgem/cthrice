// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Iterator over elements. */
template<typename IteratorType, typename Element>
concept Iterator =
    requires(IteratorType const& constIterator, IteratorType& iterator)
{
    {
        constIterator.hasNext()
        } -> std::same_as<bool>;
    {
        iterator.next()
        } -> std::convertible_to<Element&>;
};

/* Iterator that is made of two different iterators. */
template<
    typename First,
    typename Second,
    Iterator<First>  FirstIterator,
    Iterator<Second> SecondIterator>
struct ZippedIterator {
    /* Two elements zipped together. */
    struct Zip {
        /* First element. */
        First& first;
        /* Second element. */
        Second& second;
    };

    /* Whether there is an element left. */
    [[nodiscard]] constexpr bool hasNext() const noexcept
    {
        return first.hasNext() && second.hasNext();
    }

    /* Iterate and return a reference to the next element. */
    [[nodiscard]] constexpr Zip next() noexcept
    {
        return {.first = first.next(), .second = second.next()};
    }

private:

    /* First iterator. */
    FirstIterator first;
    /* Second iterator. */
    SecondIterator second;

    /* Zip the iterators together. */
    friend ZippedIterator<First, Second, FirstIterator, SecondIterator>
    zip(FirstIterator& first, SecondIterator& second) noexcept
    {
        return {.first = first, .second = second};
    }
};
} // namespace cthrice
