// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "expect.cc"
#include "format.cc"

#include <iostream>
#include <span>

/* Print the arguments. */
auto print_arguments(std::span<char const* const> const& arguments) noexcept
{
    cthrice::print(
        "Thrice C Transpiler\n"
        "Running with arguments:\n");
    for (std::size_t i{0}; i < arguments.size(); i++) {
        cthrice::print("[%] {%}\n", i, arguments[i]);
    }
    cthrice::print("\n");
}

/* Launch the compiler. */
auto main(
    int const                argumentCount,
    char const* const* const argumentValues) noexcept -> int
{
    std::span<char const* const> const arguments{
        argumentValues,
        static_cast<std::size_t>(argumentCount)};
    print_arguments(arguments);

    cthrice::unexpected("Hello, hello, hello!");

    return 0;
}
