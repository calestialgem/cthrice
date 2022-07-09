// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "patlak/token.cc"
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
    List<B8> bfr{};
    auto     res = file::load(bfr, argv[1]);
    bfr          = res.bfr;
    View file    = res.file;

    List<patlak::Token> tkns{};

    while (view::finite(file)) {
        View<B8> line = {.bgn = file.bgn, .end = view::find(file, '\n')};
        file.bgn      = line.end + 1;
        if (view::size(line) >= 2 && view::at(line, 0) == '/' &&
            view::at(line, 1) == '/') {
            continue;
        }
        tkns = list::clear(tkns);
        tkns = patlak::token::lex(tkns, line);
        patlak::token::print_all(list::view(tkns));
    }

    tkns = list::free(tkns);
    bfr  = list::free(bfr);
}
