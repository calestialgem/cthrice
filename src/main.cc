// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/scalar.cc"

#include <array>
#include <cstdio>

using namespace cthrice;

/* Print the character slice. */
constexpr void println(Slice<c8 const> const& str) noexcept
{
    std::printf("%.*s\n", (int)str.size(), str.get(0));
}

/* Entry to the compiler. */
i32 main(i32 const, c8 const* const* const) noexcept
{
    println(Slice{"Hello, hello, hello!"}.split(',').lead);
    println(Slice{"Hello, hello, hello!"}.split('h').trail);
    println(Slice{"Hello, hello, hello!"}.split('!').trail);
    println(Slice{"Hello, hello, hello!"}.split('a').lead);
    println(Slice{"Hello, hello, hello!"}.split('a').trail);
    return 0;
}
