// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buf.c"
#include "lex.c"
#include "str.c"

#include <stdio.h>
#include <stdlib.h>

int main(int const argumentCount, const char* const* argumentValues)
{
    if (argumentCount < 2) {
        printf("Provide a Thrice file!");
    }
    const size_t CAPACITY = 1024;
    struct buf   buffer   = thriceBufferCreate(CAPACITY);

    for (int i = 1; i < argumentCount; i++) {
        buffer = thriceBufferClear(buffer);
        buffer = thriceBufferAppendFile(buffer, argumentValues[i]);

        struct str src = thriceBufferView(buffer);
        struct lex lex = thriceLex(src);

        while (lex.tkn.typ != THRICE_TOKEN_EOF) {
            thriceTokenPrint(lex.tkn);
            lex = thriceLex(lex.src);
        }
    }

    thriceBufferDestroy(buffer);
    return EXIT_SUCCESS;
}
