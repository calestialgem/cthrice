// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_TOKEN_H
#define CTHRICE_TOKEN_H 1

#include "cthrice_buffer.h"
#include "cthrice_string.h"
#include "cthrice_types.h"

typedef enum {
    CTHRICE_TOKEN_SEMICOLON,
    CTHRICE_TOKEN_OPENING_BRACKET,
    CTHRICE_TOKEN_CLOSING_BRACKET,
    CTHRICE_TOKEN_OPENING_SQUARE_BRACKET,
    CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET,
    CTHRICE_TOKEN_OPENING_CURLY_BRACKET,
    CTHRICE_TOKEN_CLOSING_CURLY_BRACKET,
    CTHRICE_TOKEN_INTEGER,
    CTHRICE_TOKEN_KEYWORD_INT8,
    CTHRICE_TOKEN_KEYWORD_INT16,
    CTHRICE_TOKEN_KEYWORD_INT32,
    CTHRICE_TOKEN_KEYWORD_INT64,
    CTHRICE_TOKEN_KEYWORD_UNT8,
    CTHRICE_TOKEN_KEYWORD_UNT16,
    CTHRICE_TOKEN_KEYWORD_UNT32,
    CTHRICE_TOKEN_KEYWORD_UNT64,
    CTHRICE_TOKEN_KEYWORD_STR,
    CTHRICE_TOKEN_KEYWORD_RETURN,
    CTHRICE_TOKEN_IDENTIFIER,
    CTHRICE_TOKEN_END_OF_FILE
} Cthrice_Token_Type;

#define CTHRICE_KEYWORD_COUNT 10
extern ichr* const CTHRICE_KEYWORDS[CTHRICE_KEYWORD_COUNT];

typedef struct {
    Cthrice_Token_Type typ;
    Cthrice_String     val;
} Cthrice_Token;

ichr*          cthrice_token_static_name(Cthrice_Token_Type typ);
Cthrice_String cthrice_token_name(Cthrice_Token_Type typ);
void           cthrice_token_print(Cthrice_Token tkn);

#endif // CTHRICE_TOKEN_H
