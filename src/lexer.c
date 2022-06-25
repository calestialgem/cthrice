// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_LEXER
#define CTHRICE_LEXER

#include "error.c"
#include "string.c"

#include <stdbool.h>
#include <stdint.h>
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
    CTHRICE_TOKEN_KEYWORD_SIZE,
    CTHRICE_TOKEN_KEYWORD_STRING,
    CTHRICE_TOKEN_KEYWORD_RETURN,
    CTHRICE_TOKEN_IDENTIFIER,
    CTHRICE_TOKEN_END_OF_FILE
} Cthrice_Token_Type;

const char* cthrice_token_name(const Cthrice_Token_Type typ)
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
        case CTHRICE_TOKEN_KEYWORD_SIZE:
            return "sz keyword";
        case CTHRICE_TOKEN_KEYWORD_STRING:
            return "str keyword";
        case CTHRICE_TOKEN_KEYWORD_RETURN:
            return "return keyword";
        case CTHRICE_TOKEN_IDENTIFIER:
            return "identifier";
        case CTHRICE_TOKEN_END_OF_FILE:
            return "end-of-file";
        default:
            return "unkown-token";
    }
}

typedef struct {
    Cthrice_Token_Type typ;
    Cthrice_String     val;
} Cthrice_Token;

void cthrice_token_print(const Cthrice_Token tkn)
{
    printf("%s", cthrice_token_name(tkn.typ));
    switch (tkn.typ) {
        case CTHRICE_TOKEN_SEMICOLON:
        case CTHRICE_TOKEN_OPENING_BRACKET:
        case CTHRICE_TOKEN_CLOSING_BRACKET:
        case CTHRICE_TOKEN_OPENING_SQUARE_BRACKET:
        case CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET:
        case CTHRICE_TOKEN_OPENING_CURLY_BRACKET:
        case CTHRICE_TOKEN_CLOSING_CURLY_BRACKET:
        case CTHRICE_TOKEN_KEYWORD_SIZE:
        case CTHRICE_TOKEN_KEYWORD_STRING:
        case CTHRICE_TOKEN_KEYWORD_RETURN:
        case CTHRICE_TOKEN_END_OF_FILE:
            break;
        case CTHRICE_TOKEN_INTEGER:
        case CTHRICE_TOKEN_IDENTIFIER:
        default:
            printf(" {%.*s}", (int)cthrice_string_length(tkn.val), tkn.val.bgn);
    }
    printf("\n");
}

typedef struct {
    Cthrice_Token  tkn;
    Cthrice_String src;
} Cthrice_Lexed_Token;

Cthrice_String cthrice_lexer_word(const Cthrice_String src)
{
    const Cthrice_String trm = cthrice_string_trim(src);
    return cthrice_string_first_word(trm);
}

Cthrice_Lexed_Token cthrice_lexer_create(
    const Cthrice_Token_Type typ,
    const Cthrice_String     val,
    const Cthrice_String     src)
{
    return (Cthrice_Lexed_Token){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.bgn = val.end, .end = src.end}
    };
}

Cthrice_Lexed_Token
cthrice_lexer_number(const Cthrice_String word, const Cthrice_String src)
{
    const size_t len = cthrice_string_length(word);

    if (len == 1) {
        return cthrice_lexer_create(CTHRICE_TOKEN_INTEGER, word, src);
    }

    bool (*digit)(uint8_t) = &cthrice_digit;
    const uint8_t* end     = word.bgn + 1;

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

bool cthrice_lexer_id_char(const uint8_t chr)
{
    return chr == '_' || cthrice_letter(chr);
}

Cthrice_Lexed_Token cthrice_lex(const Cthrice_String src)
{
    const Cthrice_String word = cthrice_lexer_word(src);

    if (!cthrice_string_length(word)) {
        return cthrice_lexer_create(CTHRICE_TOKEN_END_OF_FILE, word, src);
    }

    { // Check for punctuation.
        const Cthrice_String puncs = cthrice_string_static(";()[]{}");
        const uint8_t* pos = cthrice_string_first_pos_chr(puncs, *word.bgn);

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
        const uint8_t* end = word.bgn + 1;
        while (end < word.end &&
               (cthrice_lexer_id_char(*end) || cthrice_digit(*end))) {
            end++;
        }
        const Cthrice_String val = {.bgn = word.bgn, .end = end};
#define CTHRICE_LEXER_KEYWORD_COUNT 3
        const Cthrice_String keywords[CTHRICE_LEXER_KEYWORD_COUNT] = {
            cthrice_string_static("sz"),
            cthrice_string_static("str"),
            cthrice_string_static("return"),
        };
        for (size_t i = 0; i < CTHRICE_LEXER_KEYWORD_COUNT; i++) {
            if (cthrice_string_equals(keywords[i], val)) {
                return cthrice_lexer_create(
                    CTHRICE_TOKEN_KEYWORD_SIZE + i,
                    val,
                    src);
            }
        }
        return cthrice_lexer_create(CTHRICE_TOKEN_IDENTIFIER, val, src);
    }

    const Cthrice_String val = cthrice_string_part(word, 0, 1);
    return cthrice_lexer_create(-1, val, src);
}

#endif // CTHRICE_LEXER
