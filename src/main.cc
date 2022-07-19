// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <span>
#include <string_view>
#include <vector>

/* Arguments given to the program. */
using Arguments = std::span<char const* const>;

/* Print the arguments. */
auto print_arguments(Arguments const& arguments) noexcept
{
    std::cout << "Thrice C Transpiler\n"
              << "Running with arguments:\n";
    for (std::size_t i{0}; i < arguments.size(); i++) {
        std::cout << '[' << i << "] {" << arguments[i] << "}\n";
    }
    std::cout << std::endl;
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

    return 0;
}
