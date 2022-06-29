// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error.hh"
#include "string/api.hh"
#include "view.hh"

using namespace cthrice;

int main(int argc, char** argv)
{
    cthrice_check(argc < 2, "Provide a Thrice file!");
    auto path = cstr(argv[1]);
    println(path);
}
