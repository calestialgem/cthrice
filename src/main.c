// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer.c"
#include "format.c"
#include "lexer.c"
#include "scalar.c"
#include "string.c"

#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Provide a Thrice file!");
    }
    const uptr     CAPPACITY = 1024;
    Cthrice_Buffer buf       = cthrice_buffer_create(CAPPACITY);

    for (int i = 1; i < argc; i++) {
        Cthrice_String name = cthrice_string_static(argv[i]);
        cthrice_format_println("Name of the file: %s", name);

        buf = cthrice_buffer_clear(buf);
        buf = cthrice_buffer_append_file(buf, argv[i]);

        Cthrice_String      src = cthrice_buffer_view(buf);
        Cthrice_Lexed_Token lex = cthrice_lex(src);

        while (lex.tkn.typ != CTHRICE_TOKEN_END_OF_FILE) {
            cthrice_token_print(lex.tkn);
            lex = cthrice_lex(lex.src);
        }
    }

    cthrice_buffer_destroy(buf);
    return EXIT_SUCCESS;
}
