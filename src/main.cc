// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer.hh"
#include "error.hh"
#include "string/api.hh"
#include "view.hh"

#include <cstdio>

using namespace cthrice;

int main(int argc, char** argv)
{
    cthrice_check(argc < 2, "Provide a Thrice file!");
    auto         path = cstr(argv[1]);
    Buffer<char> bfr{};
    bfr = put(bfr, cstr("Name of the file: "));
    bfr = put(bfr, path);
    println(view(bfr));
    bfr = destory(bfr);
}
