// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_LEXER_H
#define CTHRICE_LEXER_H 1

#include "cthrice_buffer.h"
#include "cthrice_string.h"
#include "cthrice_token.h"
#include "cthrice_types.h"

typedef struct {
    Cthrice_Token  lst;
    Cthrice_String src;
} Cthrice_Lexer;

Cthrice_String cthrice_lexer_word(Cthrice_String src);
Cthrice_Lexer  cthrice_lexer_create(
     Cthrice_Token_Type typ,
     Cthrice_String     val,
     Cthrice_String     src);

Cthrice_Lexer cthrice_lexer_number(Cthrice_String word, Cthrice_String src);
bool          cthrice_lexer_id_char(uchr chr);
Cthrice_Lexer cthrice_lexer_next(Cthrice_String src);

typedef struct {
    Cthrice_Token* restrict bgn;
    Cthrice_Token* restrict end;
} Cthrice_Lex;

Cthrice_Lex cthrice_lex(Cthrice_String src);
Cthrice_Lex cthrice_lex_destory(Cthrice_Lex lex);

#endif // CTHRICE_LEXER_H
