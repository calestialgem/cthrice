// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Can append characters to it. */
template<typename Type>
concept Appendable = requires(Type& appendable, char const character)
{
    {
        append(appendable, character)
        } -> std::same_as<void>;
};
} // namespace cthrice
