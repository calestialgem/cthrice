// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Iterator over elements. */
template<typename IteratorType, typename Element>
concept Iterator = requires(IteratorType iterator)
{
    {
        iterator.get()
        } -> std::convertible_to<Element&>;
    {
        iterator.next()
        } -> std::same_as<void>;
    {
        iterator.hasNext()
        } -> std::same_as<bool>;
};
} // namespace cthrice
