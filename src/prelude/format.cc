// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <iostream>

namespace cthrice
{
/* Print the format string. */
constexpr void format(char const* format) noexcept
{
    char character{0};
    while ((character = *format) != 0) {
        format++;
        std::cout << character;
    }
}

/* Print the values according to formatting. */
template<typename First, typename... Rest>
constexpr void
format(char const* format, First const& first, Rest const&... rest) noexcept
{
    char character{0};
    while ((character = *format) != 0) {
        format++;
        constexpr char INDICATOR = '%';
        if (character == INDICATOR) {
            if (!*format || *format == INDICATOR) {
                format++;
            } else {
                std::cout << first;
                if constexpr (sizeof...(rest) == 0) {
                    cthrice::format(format);
                } else {
                    cthrice::format(format, rest...);
                }
                break;
            }
        }
        std::cout << character;
    }
}
} // namespace cthrice
