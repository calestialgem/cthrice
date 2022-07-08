// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "types.cc"

#include <cstdio>
#include <exception>

namespace cthrice
{
    /* Whether the transpiler is in debugging mode. */
    constexpr bool DEBUG_MODE = true;

    /* Check whether the condition holds or not. Terminates if the condition is
     * false. Prints the error message with the file and line number. */
    constexpr void check(
        bool      cnd,
        const b8* msg,
        const b8* file = __builtin_FILE(),
        u32       line = __builtin_LINE())
    {
        if (!cnd) {
            std::printf("%s:%u: error: %s", file, line, msg);
            std::terminate();
        }
    }
} // namespace cthrice
