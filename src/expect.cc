// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "format.cc"
#include "source_location.cc"

#include <exception>
#include <string_view>

namespace cthrice
{
/* If the condition does not hold, print the error message with the source
 * location information and terminate. */
constexpr void expect(
    bool const              condition,
    std::string_view const& message,
    SourceLocation          sourceLocation = SourceLocation{}) noexcept
{
    if (!condition) {
        print("%: error: %\n", sourceLocation, message);
        std::terminate();
    }
}

/* Print the error message with the source location information and terminate.
 */
constexpr void unexpected(
    std::string_view const& message,
    SourceLocation          sourceLocation = SourceLocation{}) noexcept
{
    expect(false, message, sourceLocation);
}
} // namespace cthrice
