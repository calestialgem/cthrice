// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/list.cc"
#include "prelude/scalar.cc"

namespace cthrice
{
/* Mapping from hashable keys to values. */
template<typename Key, typename Value>
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
} // namespace cthrice
