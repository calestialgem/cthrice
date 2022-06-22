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
    const size_t CAP = 1024;
    thr_buf      buf = thriceBufferCreate(CAP);

    for (int i = 1; i < argc; i++) {
        buf = thriceBufferClear(buf);
        buf = thriceBufferAppendFile(buf, argv[i]);

        thr_str     src = thriceBufferView(buf);
        thr_lxd_tkn lex = thriceLex(src);

        while (lex.tkn.typ != THR_TOKEN_END_OF_FILE) {
            thriceTokenPrint(lex.tkn);
            lex = thriceLex(lex.src);
        }
    }

    thriceBufferDestroy(buf);
    return EXIT_SUCCESS;
}
