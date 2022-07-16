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

/* Border before the first node of the tree. */
template<typename Element>
[[nodiscard]] constexpr typename Tree<Element>::Node*
first(Tree<Element> const& tree) noexcept
{
    return first(tree.nodes);
}

/* Border after the last node of the tree. */
template<typename Element>
[[nodiscard]] constexpr typename Tree<Element>::Node*
last(Tree<Element> const& tree) noexcept
{
    return last(tree.nodes);
}
} // namespace cthrice
