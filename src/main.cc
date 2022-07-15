// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/expect.cc"
#include "prelude/range.cc"
#include "prelude/scalar.cc"
#include "prelude/view.cc"

#include <array>
#include <iostream>

using namespace cthrice;

/* Print the characters in the range. */
template<Range<const c8> RangeType>
constexpr void println(RangeType const& range) noexcept
{
    for (const c8* i = range.begin; i < range.end; i++) {
        std::cout << *i;
    }
    std::cout << std::endl;
}

/* Entry to the compiler. */
int main(int const argc, char const* const* const argv)
{
    // Print arguments.
    std::cout << "Thrice C Transpiler\n"
              << "Running with arguments:\n";
    for (int i{0}; i < argc; i++) {
        std::cout << "[" << i << "] {" << argv[i] << "}\n";
    }
    std::cout << std::endl;

    println(view_terminated("Hello, hello, hello!"));

    std::array text = {'H', 'e', 'l', 'l', 'o', '!'};
    println(view_static(text));

    std::array const consttext = {'H', 'e', 'l', 'l', 'o', '!'};
    println(view_static(consttext));

    return 0;
}
