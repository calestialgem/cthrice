// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "source_location.cc"

#include <cstddef>
#include <exception>
#include <iostream>
#include <string_view>

namespace cthrice
{
/* If the condition does not hold, print the error message with the source
 * location information and terminate. */
constexpr void expect(
    bool const        condition,
    char const* const message,
    SourceLocation    sourceLocation = SourceLocation{}) noexcept
{
    if (!condition) {
        std::cout << sourceLocation << ": error: " << message << std::endl;
        std::terminate();
    }
}

/* Print the error message with the source location information and terminate.
 */
constexpr void unexpected(
    char const* const message,
    SourceLocation    sourceLocation = SourceLocation{}) noexcept
{
    expect(false, message, sourceLocation);
}
} // namespace cthrice
