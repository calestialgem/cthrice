// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_LEXER_H
#define CTHRICE_LEXER_H 1

#include "string/mod.h"

/** Holds the token rules. */
typedef struct Cthrice_Lexer Cthrice_Lexer;

/** Load a lexer from the rules file at the given path. */
const Cthrice_Lexer* cthrice_lexer_create(const char* pth);
/** Free the memory allocated by the lexer. */
void cthrice_lexer_destroy(const Cthrice_Lexer* lxr);

/** A token. */
typedef struct {
    Cthrice_String typ;
    Cthrice_String val;
} Cthrice_Token;

/** Lex result: token array. */
typedef struct {
    const Cthrice_Token* restrict bgn;
    const Cthrice_Token* restrict end;
} Cthrice_Lex;

/** Lex the given string to a token array. */
Cthrice_Lex cthrice_lex_create(const Cthrice_Lexer* lxr, Cthrice_String src);
/** Free the momery allocated by the token array. */
void cthrice_lex_destroy(Cthrice_Lex lex);

#endif // CTHRICE_LEXER_H
