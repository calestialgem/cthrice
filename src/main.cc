// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>

/* Compile the file. */
void compile(char const* const path) noexcept
{
    std::cout << "Compiling " << path << "...\n";
    std::cout.flush();
}

/* Entry to the compiler. */
int main(int const argc, char const* const* const argv) noexcept
{
    // Print arguments.
    std::cout << "Thrice C Transpiler\nRunning with arguments:\n";
    for (int i{0}; i < argc; i++) {
        std::cout << "[" << i << "] {" << argv[i] << "}\n";
    }
    std::cout << std::endl;

    if (argc < 2) {
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        compile(argv[i]);
    }

    std::cout.flush();
    return 0;
}
