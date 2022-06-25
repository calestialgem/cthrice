// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THR_LEXER
#define THR_LEXER

#include "error.c"
#include "string.c"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    THR_TOKEN_SEMICOLON,
    THR_TOKEN_OPENING_BRACKET,
    THR_TOKEN_CLOSING_BRACKET,
    THR_TOKEN_OPENING_SQUARE_BRACKET,
    THR_TOKEN_CLOSING_SQUARE_BRACKET,
    THR_TOKEN_OPENING_CURLY_BRACKET,
    THR_TOKEN_CLOSING_CURLY_BRACKET,
    THR_TOKEN_INTEGER,
    THR_TOKEN_KEYWORD_SIZE,
    THR_TOKEN_KEYWORD_STRING,
    THR_TOKEN_KEYWORD_RETURN,
    THR_TOKEN_IDENTIFIER,
    THR_TOKEN_END_OF_FILE
} thr_tkn_typ;

const char* thriceTokenName(const thr_tkn_typ typ)
{
    switch (typ) {
        case THR_TOKEN_SEMICOLON:
            return "semicolon";
        case THR_TOKEN_OPENING_BRACKET:
            return "opening-bracket";
        case THR_TOKEN_CLOSING_BRACKET:
            return "closing-bracket";
        case THR_TOKEN_OPENING_SQUARE_BRACKET:
            return "opening-square-bracket";
        case THR_TOKEN_CLOSING_SQUARE_BRACKET:
            return "closing-square-bracket";
        case THR_TOKEN_OPENING_CURLY_BRACKET:
            return "opening-curly-bracket";
        case THR_TOKEN_CLOSING_CURLY_BRACKET:
            return "closing-curly-bracket";
        case THR_TOKEN_INTEGER:
            return "integer";
        case THR_TOKEN_KEYWORD_SIZE:
            return "sz keyword";
        case THR_TOKEN_KEYWORD_STRING:
            return "str keyword";
        case THR_TOKEN_KEYWORD_RETURN:
            return "return keyword";
        case THR_TOKEN_IDENTIFIER:
            return "identifier";
        case THR_TOKEN_END_OF_FILE:
            return "end-of-file";
        default:
            return "unkown-token";
    }
}

typedef struct {
    thr_tkn_typ    typ;
    Cthrice_String val;
} thr_tkn;

void thriceTokenPrint(const thr_tkn tkn)
{
    printf("%s", thriceTokenName(tkn.typ));
    switch (tkn.typ) {
        case THR_TOKEN_SEMICOLON:
        case THR_TOKEN_OPENING_BRACKET:
        case THR_TOKEN_CLOSING_BRACKET:
        case THR_TOKEN_OPENING_SQUARE_BRACKET:
        case THR_TOKEN_CLOSING_SQUARE_BRACKET:
        case THR_TOKEN_OPENING_CURLY_BRACKET:
        case THR_TOKEN_CLOSING_CURLY_BRACKET:
        case THR_TOKEN_KEYWORD_SIZE:
        case THR_TOKEN_KEYWORD_STRING:
        case THR_TOKEN_KEYWORD_RETURN:
        case THR_TOKEN_END_OF_FILE:
            break;
        case THR_TOKEN_INTEGER:
        case THR_TOKEN_IDENTIFIER:
        default:
            printf(" {%.*s}", (int)cthrice_string_length(tkn.val), tkn.val.bgn);
    }
    printf("\n");
}

typedef struct {
    thr_tkn        tkn;
    Cthrice_String src;
} thr_lxd_tkn;

Cthrice_String thriceLexerWord(const Cthrice_String src)
{
    const Cthrice_String trm = cthrice_string_trim(src);
    return cthrice_string_first_word(trm);
}

thr_lxd_tkn thriceLexerCreate(
    const thr_tkn_typ    typ,
    const Cthrice_String val,
    const Cthrice_String src)
{
    return (thr_lxd_tkn){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.bgn = val.end, .end = src.end}
    };
}

thr_lxd_tkn
thriceLexerNumber(const Cthrice_String word, const Cthrice_String src)
{
    const size_t len = cthrice_string_length(word);

    if (len == 1) {
        return thriceLexerCreate(THR_TOKEN_INTEGER, word, src);
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
                return thriceLexerCreate(
                    THR_TOKEN_INTEGER,
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

    return thriceLexerCreate(
        THR_TOKEN_INTEGER,
        (Cthrice_String){.bgn = word.bgn, .end = end},
        src);
}

bool thriceLexerIdChr(const uint8_t chr)
{
    return chr == '_' || cthrice_letter(chr);
}

thr_lxd_tkn thriceLex(const Cthrice_String src)
{
    const Cthrice_String word = thriceLexerWord(src);

    if (!cthrice_string_length(word)) {
        return thriceLexerCreate(THR_TOKEN_END_OF_FILE, word, src);
    }

    { // Check for punctuation.
        const Cthrice_String puncs = cthrice_string_static(";()[]{}");
        const uint8_t* pos = cthrice_string_first_pos_chr(puncs, *word.bgn);

        if (pos < puncs.end) {
            return thriceLexerCreate(
                THR_TOKEN_SEMICOLON + (pos - puncs.bgn),
                cthrice_string_part(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (cthrice_digit(*word.bgn)) {
        return thriceLexerNumber(word, src);
    }

    // Check for identifier or keyword.
    if (thriceLexerIdChr(*word.bgn)) {
        const uint8_t* end = word.bgn + 1;
        while (end < word.end &&
               (thriceLexerIdChr(*end) || cthrice_digit(*end))) {
            end++;
        }
        const Cthrice_String val = {.bgn = word.bgn, .end = end};
#define THR_LEXER_KEYWORD_COUNT 3
        const Cthrice_String keywords[THR_LEXER_KEYWORD_COUNT] = {
            cthrice_string_static("sz"),
            cthrice_string_static("str"),
            cthrice_string_static("return"),
        };
        for (size_t i = 0; i < THR_LEXER_KEYWORD_COUNT; i++) {
            if (cthrice_string_equals(keywords[i], val)) {
                return thriceLexerCreate(THR_TOKEN_KEYWORD_SIZE + i, val, src);
            }
        }
        return thriceLexerCreate(THR_TOKEN_IDENTIFIER, val, src);
    }

    const Cthrice_String val = cthrice_string_part(word, 0, 1);
    return thriceLexerCreate(-1, val, src);
}

#endif // THR_LEXER
