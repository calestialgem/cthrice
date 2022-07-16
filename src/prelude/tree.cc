// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/list.cc"
#include "prelude/scalar.cc"

namespace cthrice
{
/* Dynamic, hierarchical elements. */
template<typename Element>
struct Tree {
    /* Node in the tree. */
    struct Node {
        /* Element in this node. */
        Element element;
        /* Amount of childeren. */
        ix childeren;
    };
    /* Data. */
    List<Node> nodes;
};
} // namespace cthrice
