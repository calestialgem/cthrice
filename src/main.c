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
    ThriceBuffer bfr = thriceBufferCreate(CAP);

    for (int i = 1; i < argc; i++) {
        bfr = thriceBufferClear(bfr);
        bfr = thriceBufferAppendFile(bfr, argv[i]);

        ThriceString     src = thriceBufferView(bfr);
        ThriceLexedToken lex = thriceLex(src);

        while (lex.tkn.typ != THRICE_TOKEN_EOF) {
            thriceTokenPrint(lex.tkn);
            lex = thriceLex(lex.src);
        }
    }

    thriceBufferDestroy(bfr);
    return EXIT_SUCCESS;
}
