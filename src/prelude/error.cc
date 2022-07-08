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

    /* Print the message and type with the file and line number. */
    void
    log(const b8* msg,
        const b8* typ,
        const b8* file = __builtin_FILE(),
        u32       line = __builtin_LINE())
    {
        std::printf("%s:%u: %s: %s\n", file, line, typ, msg);
        std::terminate();
    }

    /* Check whether the condition holds or not. Terminates if the condition is
     * false. Prints the error message with the file and line number. */
    void check(
        bool      cnd,
        const b8* msg,
        const b8* file = __builtin_FILE(),
        u32       line = __builtin_LINE())
    {
        if (!cnd) {
            log(msg, "error", file, line);
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
            if (!cnd) {
                log(msg, "debug", file, line);
            }
        }
    }
} // namespace cthrice
