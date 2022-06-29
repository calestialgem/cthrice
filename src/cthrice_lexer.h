// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_LEXER_H
#define CTHRICE_LEXER_H 1

#include "cthrice_string.h"
#include "cthrice_token.h"

typedef struct {
    Cthrice_Token* restrict bgn;
    Cthrice_Token* restrict end;
} Cthrice_Lex;

Cthrice_Lex cthrice_lex(Cthrice_String src);
Cthrice_Lex cthrice_lex_destory(Cthrice_Lex lex);

#endif // CTHRICE_LEXER_H
