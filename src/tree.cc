// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <stack>
#include <utility>
#include <vector>

namespace cthrice
{
/* Hierarchical representation of contiguous elements. */
template<typename Element>
struct Tree {
    /* Hierarchical representation of an element. */
    struct Node {
        /* Element. */
        [[nodiscard]] constexpr auto value() noexcept -> Element&
        {
            return value_;
        }

        /* Immutable element. */
        [[nodiscard]] constexpr auto value() const noexcept -> Element const&
        {
            return value_;
        }

        /* Number of childeren it has. */
        [[nodiscard]] constexpr auto childeren() const noexcept -> std::size_t
        {
            return childeren_;
        }

    private:

        template<typename... Arguments>
        [[nodiscard]] constexpr explicit Node(Arguments&&... arguments) noexcept
            : value_{std::forward(arguments...)}
        {
        }

        Element     value_;
        std::size_t childeren_{0};
    };

    /* Amount of nodes. */
    [[nodiscard]] constexpr auto size() const noexcept
    {
        return nodes_.size();
    }

    /* Whether there are any nodes. */
    [[nodiscard]] constexpr auto finite() const noexcept
    {
        return size() > 0;
    }

    /* Reference to the node at the index. */
    [[nodiscard]] constexpr auto get(std::size_t index) noexcept -> Node&
    {
        return nodes_[index];
    }

    /* Immutable reference to the node at the index. */
    [[nodiscard]] constexpr auto get(std::size_t index) const noexcept
        -> Node const&
    {
        return nodes_[index];
    }

    /* Add the element to the end. */
    template<typename... Arguments>
    constexpr auto add(Arguments&&... args) noexcept
    {
        nodes_.emplace_back(std::forward(args...));
    }

    /* Helper that builds a tree top-down, left to right. */
    struct Builder {
        /* Make the last node the current parent. */
        constexpr auto push() noexcept
        {
            parents_.push(built_.size() - 1);
        }

        /* Make the previous parent as the current parent. */
        constexpr auto pop() noexcept
        {
            parents_.pop();
        }

        /* Add childeren to the current parrent. */
        template<typename... Arguments>
        constexpr auto add(Arguments&&... arguments) noexcept
        {
            built_.add(std::forward(arguments...));
            if (!parents_.empty()) {
                built_.nodes_[parents_.top()].childeren_++;
            }
        }

        /* Tree that is being built. */
        [[nodiscard]] constexpr auto built() noexcept -> Tree&
        {
            return built_;
        }

        /* Immutable tree that is being built. */
        [[nodiscard]] constexpr auto built() const noexcept -> Tree const&
        {
            return built_;
        }

    private:

        Tree&                   built_;
        std::stack<std::size_t> parents_;
    };

private:

    std::vector<Node> nodes_;
};
} // namespace cthrice
