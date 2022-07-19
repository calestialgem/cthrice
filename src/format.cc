// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <iostream>
#include <ostream>
#include <string_view>

namespace cthrice
{
/* Format indicator. */
constexpr char FORMAT_INDICATOR = '%';

/* Format the values to the output stream. */
constexpr void
format(std::ostream& out, std::string_view const& format) noexcept
{
    out << format;
    out.flush();
}

/* Format the values to the output stream. */
template<typename First, typename... Rest>
constexpr void format(
    std::ostream&    out,
    std::string_view format,
    First const&     first,
    Rest const&... rest)
{
    auto indicator = format.find_first_of(FORMAT_INDICATOR);
    out << format.substr(0, indicator);
    format.remove_prefix(indicator + 1);
    out << first;
    cthrice::format(out, format, rest...);
}

/* Print the values. */
template<typename... Values>
constexpr void print(std::string_view const& format, Values const&... values)
{
    cthrice::format(std::cout, format, values...);
}
} // namespace cthrice
