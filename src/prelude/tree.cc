// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/list.cc"
#include "prelude/types.cc"

namespace cthrice
{
    namespace tree
    {
        /* A node in the tree. */
        template<typename T>
        struct Node {
            /* Value in this node. */
            T val;
            /* Amount of childeren. */
            Ix chi;
        };
    } // namespace tree

    /* Nonlinear structure stored in linear memory. */
    template<typename T>
    struct Tree {
        List<tree::Node<T>> nodes;
    };

    namespace tree
    {
        /* Amount of nodes. */
        template<typename T>
        Ix size(Tree<T> tree)
        {
            return list::size(tree.nodes);
        }

        /* Amount of allocated node memory. */
        template<typename T>
        Ix capacity(Tree<T> tree)
        {
            return list::capacity(tree.nodes);
        }

        /* Amount of unused node memory. */
        template<typename T>
        Ix space(Tree<T> tree)
        {
            return list::space(tree.nodes);
        }

        /* Whether there are any nodes. */
        template<typename T>
        bool finite(Tree<T> tree)
        {
            return size(tree) > 0;
        }

        /* Deallocate the memory. */
        template<typename T>
        [[nodiscard]] Tree<T> free(Tree<T> tree)
        {
            tree.nodes = list::free(tree.nodes);
            return tree;
        }

        /* Remove all the nodes. Keeps the memory. */
        template<typename T>
        [[nodiscard]] Tree<T> clear(Tree<T> tree)
        {
            tree.nodes = list::clear(tree.nodes);
            return tree;
        }

        /* Make sure the space is at least as big as the amount. Grows if
         * necessary by at least the half of the current capacity. */
        template<typename T>
        [[nodiscard]] Tree<T> reserve(Tree<T> tree, Ix amt)
        {
            tree.nodes = list::reserve(tree.nodes, amt);
            return tree;
        }
    } // namespace tree
} // namespace cthrice
