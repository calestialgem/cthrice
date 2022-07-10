// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "patlak/token.cc"
#include "prelude/error.cc"
#include "prelude/file.cc"
#include "prelude/list.cc"
#include "prelude/map.cc"
#include "prelude/slice.cc"
#include "prelude/types.cc"

#include <cstdio>

using namespace cthrice;

int main(int argc, char** argv)
{
    check(argc >= 2, "Provide a Thrice file!");
    List<B8> bfr{};
    auto     res = file::load(bfr, argv[1]);
    bfr          = res.bfr;
    Slice file   = res.file;

    List<patlak::Token> tkns{};

    while (slice::finite(file)) {
        Slice<const B8> line = {
            .bgn = file.bgn,
            .end = slice::first(file, '\n')};
        file.bgn = line.end + 1;
        if (slice::size(line) >= 2 && slice::at(line, 0) == '/' &&
            slice::at(line, 1) == '/') {
            continue;
        }
        tkns = list::clear(tkns);
        tkns = patlak::token::lex(tkns, line);
        patlak::token::print_all(list::slice(tkns));
    }

    tkns = list::free(tkns);
    bfr  = list::free(bfr);
}
