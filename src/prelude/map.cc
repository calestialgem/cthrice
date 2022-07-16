// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/hashable.cc"
#include "prelude/list.cc"
#include "prelude/scalar.cc"

namespace cthrice
{
/* Mapping from hashable keys to values. */
template<Hashable Key, typename Value>
struct Map {
    /* Pair of a key and its value. */
    struct Pair {
        /* Hashable key. */
        Key key;
        /* Corresponding value. */
        Value value;
    };
    /* Pair indicies that correspond to hashes. */
    List<ix> indicies;
    /* Data. */
    List<Pair> pairs;
};

/* Border before the first pair of the map. */
template<Hashable Key, typename Value>
[[nodiscard]] constexpr typename Map<Key, Value>::Pair*
first(Map<Key, Value> const& map) noexcept
{
    return first(map.pairs);
}

/* Border after the last pair of the map. */
template<Hashable Key, typename Value>
[[nodiscard]] constexpr typename Map<Key, Value>::Pair*
last(Map<Key, Value> const& map) noexcept
{
    return last(map.pairs);
}
} // namespace cthrice
