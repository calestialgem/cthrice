// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buf.c"
#include "lex.c"
#include "str.c"

#include <stdio.h>
#include <stdlib.h>

int main(int const argc, const char* const* argv)
{
    if (argc < 2) {
        printf("Provide a Thrice file!");
    }
    const size_t CAPACITY = 1024;
    struct buf   buf      = cthr_buf_create(CAPACITY);

    for (int i = 1; i < argc; i++) {
        buf = cthr_buf_clear(buf);
        buf = cthr_buf_file(buf, argv[i]);

        struct str src = cthr_buf_view(buf);
        struct lex lex = cthr_lex(src);

        while (lex.tkn.typ != TKN_EOF) {
            cthr_lex_print_tkn(lex.tkn);
            lex = cthr_lex(lex.src);
        }
    }

    cthr_buf_destroy(buf);
    return EXIT_SUCCESS;
}
