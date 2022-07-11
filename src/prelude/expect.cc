// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "scalar.cc"

#include <cstdio>
#include <exception>

namespace cthrice
{
    /* If the condition does not hold, print the error message with the file and
     * line number and terminate. */
    void expect(
        b8        cnd,
        c8 const* msg,
        c8 const* file = __builtin_FILE(),
        u32       line = __builtin_LINE()) noexcept
    {
        if (!cnd) {
            std::printf("%s:%u: error: %s\n", file, line, msg);
            std::terminate();
        }
    }

    /* Print the error message with the file and line number and terminate. */
    constexpr void unexpected(
        c8 const* msg,
        c8 const* file = __builtin_FILE(),
        u32       line = __builtin_LINE()) noexcept
    {
        expect(false, msg, file, line);
    }
} // namespace cthrice
