// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_buffer.h"
#include "cthrice_character.h"
#include "cthrice_error.h"
#include "cthrice_format.h"
#include "cthrice_lexer.h"
#include "cthrice_token.h"
#include "cthrice_types.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc < 2) {
        cthrice_error("Provide a Thrice file!");
    }
    const uptr     CAPPACITY = 1024;
    Cthrice_Buffer buf       = cthrice_buffer_create(CAPPACITY);

    for (int i = 1; i < argc; i++) {
        Cthrice_String name = cthrice_string_static(argv[i]);
        cthrice_format_println("Name of the file: %s", name);

        buf = cthrice_buffer_clear(buf);
        buf = cthrice_buffer_append_file(buf, argv[i]);

        Cthrice_String src = cthrice_buffer_view(buf);
        Cthrice_Lex    lex = cthrice_lex(src);

        for (const Cthrice_Token* i = lex.bgn; i < lex.end; i++) {
            cthrice_token_print(*i);
        }

        cthrice_lex_destory(lex);
    }

    cthrice_buffer_destroy(buf);
    return EXIT_SUCCESS;
}
