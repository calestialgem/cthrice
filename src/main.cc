// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>

/* Entry to the compiler. */
auto main(int const argc, char const* const* const argv) -> int
{
    // Print arguments.
    std::cout << "Thrice C Transpiler\n"
                 "Running with arguments:\n";
    for (int i{0}; i < argc; i++) {
        std::cout << "[" << i << "] {" << argv[i] << "}\n";
    }
    std::cout << std::endl;

    std::cout << "Hello, hello, hello!" << std::endl;
    return 0;
}
