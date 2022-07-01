// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer.hh"
#include "error.hh"
#include "pattern/api.hh"
#include "range.hh"
#include "string/api.hh"

#include <cstdio>

using namespace cthrice;

int main(int argc, char** argv)
{
    cthrice_check(argc < 2, "Provide a Thrice file!");
    auto         path = cstr(argv[1]);
    Buffer<char> bfr{};

    bfr = put(bfr, cstr("Name of the file: "));
    bfr = put(bfr, path);
    bfr = put(bfr, cstr("\nContents of the file:\n"));
    println(view(bfr));

    auto off = size(bfr);
    bfr      = append(bfr, path);
    println(view(bfr, off));

    bfr = destory(bfr);

    Buffer<Pattern> patterns{};
    patterns = parse(patterns, cstr("h@"), cstr("'h' 'e' 'l' 'l' 'o'"));
    patterns = parse(patterns, cstr("a@"), cstr("'a' 'a' 'a'"));
    patterns = parse(patterns, cstr("cem@"), cstr("'C' 'e' 'm'"));
    patterns = parse(patterns, cstr("cam"), cstr("'C' 'a' 'm'"));
    cthrice_check(
        !equal(cstr("h@"), match(view(patterns), cstr("hello"))),
        "Matching failed!");
    cthrice_check(
        !equal(cstr("a@"), match(view(patterns), cstr("aaa"))),
        "Matching failed!");
    cthrice_check(
        !equal(cstr("cem@"), match(view(patterns), cstr("Cem"))),
        "Matching failed!");
    cthrice_check(
        !equal(cstr(""), match(view(patterns), cstr("Cam"))),
        "Not matching failed!");
    patterns = destory(patterns);
}
