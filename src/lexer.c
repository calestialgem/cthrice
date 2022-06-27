// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_LEXER
#define CTHRICE_LEXER

#include "buffer.c"
#include "error.c"
#include "format.c"
#include "scalar.c"
#include "string.c"

#include <stdio.h>

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

ichr* cthrice_token_static_name(Cthrice_Token_Type typ)
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

Cthrice_String cthrice_token_name(Cthrice_Token_Type typ)
{
    return cthrice_string_static(cthrice_token_static_name(typ));
}

typedef struct {
    Cthrice_Token_Type typ;
    Cthrice_String     val;
} Cthrice_Token;

void cthrice_token_print(Cthrice_Token tkn)
{
    Cthrice_Buffer bfr = {};
    bfr = cthrice_format_append(bfr, "%s", cthrice_token_name(tkn.typ));
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

typedef struct {
    Cthrice_Token  tkn;
    Cthrice_String src;
} Cthrice_Lexed_Token;

Cthrice_String cthrice_lexer_word(Cthrice_String src)
{
    Cthrice_String trm = cthrice_string_trim(src);
    return cthrice_string_first_word(trm);
}

Cthrice_Lexed_Token cthrice_lexer_create(
    Cthrice_Token_Type typ,
    Cthrice_String     val,
    Cthrice_String     src)
{
    return (Cthrice_Lexed_Token){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.bgn = val.end, .end = src.end}
    };
}

Cthrice_Lexed_Token
cthrice_lexer_number(Cthrice_String word, Cthrice_String src)
{
    uptr len = cthrice_string_length(word);

    if (len == 1) {
        return cthrice_lexer_create(CTHRICE_TOKEN_INTEGER, word, src);
    }

    bool (*digit)(uchr) = &cthrice_digit;
    uchr* end           = word.bgn + 1;

    if ('0' == *word.bgn) {
        switch (*end) {
            case 'b':
                digit = &cthrice_digit_bin;
                break;
            case 'o':
                digit = &cthrice_digit_oct;
                break;
            case 'x':
                digit = &cthrice_digit_hex;
                break;
            case '_':
                break;
            default:
                return cthrice_lexer_create(
                    CTHRICE_TOKEN_INTEGER,
                    cthrice_string_part(word, 0, 1),
                    src);
        }

        if (++end >= word.end) {
            cthrice_error("Number ended after the base indicator!");
        }
    }

    bool sep = false;
    while (end < word.end && ((*end == '_' && sep) || digit(*end))) {
        sep = *end != '_';
        end++;
    }
    if (*end == '_') {
        end--;
    }

    return cthrice_lexer_create(
        CTHRICE_TOKEN_INTEGER,
        (Cthrice_String){.bgn = word.bgn, .end = end},
        src);
}

bool cthrice_lexer_id_char(uchr chr)
{
    return chr == '_' || cthrice_letter(chr);
}

Cthrice_Lexed_Token cthrice_lex(Cthrice_String src)
{
    Cthrice_String word = cthrice_lexer_word(src);

    if (!cthrice_string_length(word)) {
        return cthrice_lexer_create(CTHRICE_TOKEN_END_OF_FILE, word, src);
    }

    { // Check for punctuation.
        Cthrice_String puncs = cthrice_string_static(";()[]{}");
        uchr*          pos   = cthrice_string_first_pos_chr(puncs, *word.bgn);

        if (pos < puncs.end) {
            return cthrice_lexer_create(
                CTHRICE_TOKEN_SEMICOLON + (pos - puncs.bgn),
                cthrice_string_part(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (cthrice_digit(*word.bgn)) {
        return cthrice_lexer_number(word, src);
    }

    // Check for identifier or keyword.
    if (cthrice_lexer_id_char(*word.bgn)) {
        uchr* end = word.bgn + 1;
        while (end < word.end &&
               (cthrice_lexer_id_char(*end) || cthrice_digit(*end))) {
            end++;
        }
        Cthrice_String val = {.bgn = word.bgn, .end = end};

        for (uptr i = 0; i < CTHRICE_KEYWORD_COUNT; i++) {
            if (cthrice_string_equals(
                    cthrice_string_static(CTHRICE_KEYWORDS[i]),
                    val)) {
                return cthrice_lexer_create(
                    CTHRICE_TOKEN_KEYWORD_INT8 + i,
                    val,
                    src);
            }
        }
        return cthrice_lexer_create(CTHRICE_TOKEN_IDENTIFIER, val, src);
    }

    Cthrice_String val = cthrice_string_part(word, 0, 1);
    return cthrice_lexer_create(-1, val, src);
}

#endif // CTHRICE_LEXER
