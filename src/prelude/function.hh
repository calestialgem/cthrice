// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace cthrice
{
/* Function. */
template<typename Type, typename Return, typename... Parameters>
concept Function = requires(Type function, Parameters const&... arguments)
{
    {
        function(arguments...)
        } -> std::same_as<Return>;
};
} // namespace cthrice
