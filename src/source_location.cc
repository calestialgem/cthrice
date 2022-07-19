// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>
#include <ostream>
#include <string_view>

namespace cthrice
{
/* Location in a source file. */
struct SourceLocation {
    /* Defaults to current location. */
    [[nodiscard]] constexpr explicit SourceLocation(
        std::string_view file = __builtin_FILE(),
        std::size_t      line = __builtin_LINE()) noexcept
        : file_{file}
        , line_{line}
    {
    }

    /* Name of the file as a null-terimated string. */
    [[nodiscard]] constexpr auto file() const noexcept
    {
        return file_;
    }

    /* Line number in the file. */
    [[nodiscard]] constexpr auto line() const noexcept
    {
        return line_;
    }

private:

    /* File name. */
    std::string_view file_;
    /* Line number. */
    std::size_t line_;
};

/* Print the source location. */
constexpr auto
operator<<(std::ostream& out, SourceLocation const& sourceLocation) noexcept
    -> std::ostream&
{
    out << sourceLocation.file() << ':' << sourceLocation.line();
    return out;
}
} // namespace cthrice
