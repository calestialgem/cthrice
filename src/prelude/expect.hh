// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.hh"

#include <exception>
#include <iostream>

namespace cthrice
{
/* Location of code in a source file. */
struct SourceLocation {
    /* Source file. */
    char const* file;
    /* Line in the file. */
    u32 line;
};

/* Create a source location at the caller. Do not provide arguments for this to
 * work. */
[[nodiscard]] constexpr SourceLocation callerSourceLocation(
    char const* const file = __builtin_FILE(),
    u32 const         line = __builtin_LINE()) noexcept
{
    return {.file = file, .line = line};
}

/* If the condition does not hold, print the error message with the file and
 * line number and terminate. */
constexpr void expect(
    bool const           condition,
    char const* const    message,
    SourceLocation const location = callerSourceLocation()) noexcept
{
    if (!condition) {
        std::cerr << location.file << ":" << location.line
                  << ": error: " << message << std::endl;
        std::terminate();
    }
}

/* Print the error message with the file and line number then terminate. */
constexpr void unexpected(
    char const* const    message,
    SourceLocation const location = callerSourceLocation()) noexcept
{
    expect(false, message, location);
}
} // namespace cthrice
