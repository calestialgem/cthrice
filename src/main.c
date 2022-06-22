// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer.c"
#include "lexer.c"
#include "string.c"

#include <stdio.h>
#include <stdlib.h>

int main(int const argumentCount, const char* const* argumentValues)
{
    if (argumentCount < 2) {
        printf("Provide a Thrice file!");
    }
    const size_t CAPACITY = 1024;
    ThriceBuffer buffer   = ThriceBufferCreate(CAPACITY);

    for (int i = 1; i < argumentCount; i++) {
        buffer = ThriceBufferClear(buffer);
        buffer = ThriceBufferAppendFile(buffer, argumentValues[i]);

        ThriceString     src = ThriceBufferView(buffer);
        ThriceLexedToken lex = ThriceLex(src);

        while (lex.tkn.typ != THRICE_TOKEN_EOF) {
            ThriceTokenPrint(lex.tkn);
            lex = ThriceLex(lex.src);
        }
    }

    ThriceBufferDestroy(buffer);
    return EXIT_SUCCESS;
}
