// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/error.cc"
#include "prelude/file.cc"
#include "prelude/list.cc"
#include "prelude/map.cc"
#include "prelude/types.cc"
#include "prelude/view.cc"

#include <cstdio>

using namespace cthrice;

int main(int argc, char** argv)
{
    check(argc >= 2, "Provide a Thrice file!");
    List<b8> bfr{};
    auto     res = file::load(bfr, argv[1]);
    bfr          = res.bfr;
    View file    = res.file;
    std::printf("%.*s\n", (int)View<b8>::size(file), file.bgn);
    bfr = List<b8>::free(bfr);
}
