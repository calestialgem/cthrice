// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/expect.cc"
#include "prelude/file.cc"
#include "prelude/format.cc"
#include "prelude/list.cc"
#include "prelude/scalar.cc"

#include <array>
#include <iostream>

using namespace cthrice;

/* Compile the file. */
void compile(char const* const path) noexcept
{
    format("Compiling {%}.\n", path);

    List<char> buffer{};
    format("%\n", load(buffer, path));
    free(buffer);

    std::cout.flush();
}

/* Entry to the compiler. */
int main(int const argc, char const* const* const argv) noexcept
{
    // Print arguments.
    format("Thrice C Transpiler\nRunning with arguments:\n");
    for (int i{0}; i < argc; i++) {
        format("[%] {%}\n", i, argv[i]);
    }
    std::cout << std::endl;

    expect(argc >= 2, "Provide at least a file!");
    for (int i{1}; i < argc; i++) {
        compile(argv[i]);
    }

    std::cout.flush();
    return 0;
}
