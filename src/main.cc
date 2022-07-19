// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "expect.cc"
#include "file.cc"
#include "format.cc"
#include "patlak/lexer.cc"

#include <fstream>
#include <ios>
#include <iostream>
#include <span>
#include <sstream>
#include <string>

using namespace cthrice;

/* Compile the source file at the path. */
constexpr auto compile(char const* path) noexcept
{
    print("Compiling %...\n", path);

    std::vector<patlak::Token> tokens;

    auto tokenizer = [&tokens](std::string_view line) {
        if (line.empty() || line.starts_with("//")) {
            return;
        }

        tokens.clear();
        patlak::lex(tokens, line);
        print("Tokens: %\n", tokens);
    };

    file::process(path, tokenizer);
}

/* Print the arguments. */
constexpr auto
print_arguments(std::span<char const* const> const& arguments) noexcept
{
    print(
        "Thrice C Transpiler\n"
        "Running with arguments:\n");
    for (std::size_t i{0}; i < arguments.size(); i++) {
        print("[%] {%}\n", i, arguments[i]);
    }
    print("\n");
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

    auto paths = arguments.subspan(1);
    expect(!paths.empty(), "Provide a path to a thrice file!");
    for (auto const& path : paths) {
        compile(path);
    }

    return 0;
}
