// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_token.h"

#include "cthrice_format.h"

ichr* const CTHRICE_KEYWORDS[CTHRICE_KEYWORD_COUNT] = {
    "int8",
    "int16",
    "int32",
    "int64",
    "unt8",
    "unt16",
    "unt32",
    "unt64",
    "str",
    "return"};

static ichr* static_name(Cthrice_Token_Type typ)
{
    switch (typ) {
        case CTHRICE_TOKEN_SEMICOLON:
            return "semicolon";
        case CTHRICE_TOKEN_OPENING_BRACKET:
            return "opening-bracket";
        case CTHRICE_TOKEN_CLOSING_BRACKET:
            return "closing-bracket";
        case CTHRICE_TOKEN_OPENING_SQUARE_BRACKET:
            return "opening-square-bracket";
        case CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET:
            return "closing-square-bracket";
        case CTHRICE_TOKEN_OPENING_CURLY_BRACKET:
            return "opening-curly-bracket";
        case CTHRICE_TOKEN_CLOSING_CURLY_BRACKET:
            return "closing-curly-bracket";
        case CTHRICE_TOKEN_INTEGER:
            return "integer";
        case CTHRICE_TOKEN_KEYWORD_INT8:
        case CTHRICE_TOKEN_KEYWORD_INT16:
        case CTHRICE_TOKEN_KEYWORD_INT32:
        case CTHRICE_TOKEN_KEYWORD_INT64:
        case CTHRICE_TOKEN_KEYWORD_UNT8:
        case CTHRICE_TOKEN_KEYWORD_UNT16:
        case CTHRICE_TOKEN_KEYWORD_UNT32:
        case CTHRICE_TOKEN_KEYWORD_UNT64:
        case CTHRICE_TOKEN_KEYWORD_STR:
        case CTHRICE_TOKEN_KEYWORD_RETURN:
            return CTHRICE_KEYWORDS[typ - CTHRICE_TOKEN_KEYWORD_INT8];
        case CTHRICE_TOKEN_IDENTIFIER:
            return "identifier";
        case CTHRICE_TOKEN_END_OF_FILE:
            return "end-of-file";
        default:
            return "unkown-token";
    }
}

static Cthrice_String name(Cthrice_Token_Type typ)
{
    return cthrice_string_static(static_name(typ));
}

void cthrice_token_print(Cthrice_Token tkn)
{
    Cthrice_Buffer bfr = {0};
    bfr                = cthrice_format_append(bfr, "%s", name(tkn.typ));
    switch (tkn.typ) {
        case CTHRICE_TOKEN_SEMICOLON:
        case CTHRICE_TOKEN_OPENING_BRACKET:
        case CTHRICE_TOKEN_CLOSING_BRACKET:
        case CTHRICE_TOKEN_OPENING_SQUARE_BRACKET:
        case CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET:
        case CTHRICE_TOKEN_OPENING_CURLY_BRACKET:
        case CTHRICE_TOKEN_CLOSING_CURLY_BRACKET:
        case CTHRICE_TOKEN_KEYWORD_INT8:
        case CTHRICE_TOKEN_KEYWORD_INT16:
        case CTHRICE_TOKEN_KEYWORD_INT32:
        case CTHRICE_TOKEN_KEYWORD_INT64:
        case CTHRICE_TOKEN_KEYWORD_UNT8:
        case CTHRICE_TOKEN_KEYWORD_UNT16:
        case CTHRICE_TOKEN_KEYWORD_UNT32:
        case CTHRICE_TOKEN_KEYWORD_UNT64:
        case CTHRICE_TOKEN_KEYWORD_STR:
        case CTHRICE_TOKEN_KEYWORD_RETURN:
        case CTHRICE_TOKEN_END_OF_FILE:
            break;
        case CTHRICE_TOKEN_INTEGER:
        case CTHRICE_TOKEN_IDENTIFIER:
        default:
            bfr = cthrice_format_append(bfr, " {%s}", tkn.val);
    }
    cthrice_string_println(cthrice_buffer_view(bfr));
    bfr = cthrice_buffer_destroy(bfr);
}
