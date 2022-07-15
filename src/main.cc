// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/expect.cc"
#include "prelude/scalar.cc"
#include "prelude/view.cc"

#include <array>
#include <iostream>

using namespace cthrice;

/* Print the characters in the view. */
void println(View<char> const& string) noexcept
{
    string.consume([](char const& character) {
        std::cout << character;
    });
    std::cout << std::endl;
}

/* Entry to the compiler. */
int main(int const argc, char const* const* const argv) noexcept
{
    // Print arguments.
    std::cout << "Thrice C Transpiler\n"
              << "Running with arguments:\n";
    for (int i{0}; i < argc; i++) {
        std::cout << "[" << i << "] {" << argv[i] << "}\n";
    }
    std::cout << std::endl;

    println(View{"Hello, hello, hello!"});

    return 0;
}
