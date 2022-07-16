// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/expect.cc"
#include "prelude/file.cc"
#include "prelude/list.cc"
#include "prelude/scalar.cc"

#include <array>
#include <iostream>

using namespace cthrice;

/* Compile the file. */
void compile(char const* const path) noexcept
{
    std::cout << "Compiling " << path << std::endl;
    List<char> buffer{};
    View<char> file = load(buffer, path);
    for (const char* i = file.first; i < file.last; i++) {
        std::cout << *i;
    }
    std::cout << std::endl;
    free(buffer);
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

    expect(argc >= 2, "Provide at least a file!");
    for (int i{1}; i < argc; i++) {
        compile(argv[i]);
    }

    return 0;
}
