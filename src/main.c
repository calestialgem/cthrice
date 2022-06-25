// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer.c"
#include "lexer.c"
#include "string.c"

#include <stdio.h>
#include <stdlib.h>

int main(int const argc, const char* const* argv)
{
    if (argc < 2) {
        printf("Provide a Thrice file!");
    }
    const size_t   CAP = 1024;
    Cthrice_Buffer buf = cthrice_buffer_create(CAP);

    for (int i = 1; i < argc; i++) {
        buf = cthrice_buffer_clear(buf);
        buf = cthrice_buffer_append_file(buf, argv[i]);

        Cthrice_String src = cthrice_buffer_view(buf);
        thr_lxd_tkn    lex = thriceLex(src);

        while (lex.tkn.typ != THR_TOKEN_END_OF_FILE) {
            thriceTokenPrint(lex.tkn);
            lex = thriceLex(lex.src);
        }
    }

    cthrice_buffer_destroy(buf);
    return EXIT_SUCCESS;
}
