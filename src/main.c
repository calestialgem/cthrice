// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buf.c"
#include "lex.c"
#include "str.c"

#include <stdio.h>
#include <stdlib.h>

int main(int const argc, const char* const* argv)
{
    printf("Running Thrice Transpiler to C with arguments:\n");
    for (int i = 0; i < argc; i++) {
        printf("[%d] %s\n", i, argv[i]);
    }

    const size_t CAPACITY = 1024;
    struct buf   buf      = cthr_buf_create(CAPACITY);
    buf                   = cthr_buf_append_file(buf, "res/main.thr");

    struct str src = cthr_str_view(buf);
    struct lex lex = cthr_lex(src);

    while (lex.tkn.typ != TKN_EOF) {
        cthr_lex_print_tkn(lex.tkn);
        lex = cthr_lex(lex.src);
    }

    return EXIT_SUCCESS;
}
