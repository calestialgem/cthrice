// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "types.cc"

#include <cstdio>
#include <exception>

namespace cthrice
{
/* Macro version of the DEBUG_MODE contant. It gets the debug information from
 * the compiler. */
#ifdef NDEBUG
#    define CTHRICE_DEBUG_MODE false
#else
#    define CTHRICE_DEBUG_MODE true
#endif

    /* Whether the transpiler is in debugging mode. */
    constexpr bool DEBUG_MODE = CTHRICE_DEBUG_MODE;

    /* Print the error message with the file and line number and terminate. */
    void error(
        const b8* msg,
        const b8* file = __builtin_FILE(),
        u32       line = __builtin_LINE())
    {
        std::printf("%s:%u: error: %s\n", file, line, msg);
        std::terminate();
    }

    /* Call error with the message if the condition does not hold. */
    void check(
        bool      cnd,
        const b8* msg,
        const b8* file = __builtin_FILE(),
        u32       line = __builtin_LINE())
    {
        if (!cnd) {
            error(msg, file, line);
        }
    }

    /* Same as check, but only works in debugging mode. */
    void debug(
        bool      cnd,
        const b8* msg,
        const b8* file = __builtin_FILE(),
        u32       line = __builtin_LINE())
    {
        if (DEBUG_MODE) {
            check(cnd, msg, file, line);
        }
    }
} // namespace cthrice
