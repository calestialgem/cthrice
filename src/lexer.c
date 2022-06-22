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
    thr_tkn_typ typ;
    thr_str     val;
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
            printf(" {%.*s}", (int)thriceStringLength(tkn.val), tkn.val.beg);
    }
    printf("\n");
}

typedef struct {
    thr_tkn tkn;
    thr_str src;
} thr_lxd_tkn;

thr_str thriceLexerWord(const thr_str src)
{
    const thr_str trm = thriceStringTrim(src);
    return thriceStringFirstWord(trm);
}

thr_lxd_tkn
thriceLexerCreate(const thr_tkn_typ typ, const thr_str val, const thr_str src)
{
    return (thr_lxd_tkn){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.beg = val.end, .end = src.end}
    };
}

thr_lxd_tkn thriceLexerNumber(const thr_str word, const thr_str src)
{
    const size_t len = thriceStringLength(word);

    if (len == 1) {
        return thriceLexerCreate(THR_TOKEN_INTEGER, word, src);
    }

    bool (*digit)(uint8_t) = &thriceDigit;
    const uint8_t* end     = word.beg + 1;

    if ('0' == *word.beg) {
        switch (*end) {
            case 'b':
                digit = &thriceDigitBin;
                break;
            case 'o':
                digit = &thriceDigitOct;
                break;
            case 'x':
                digit = &thriceDigitHex;
                break;
            case '_':
                break;
            default:
                return thriceLexerCreate(
                    THR_TOKEN_INTEGER,
                    thriceStringPart(word, 0, 1),
                    src);
        }

        if (++end >= word.end) {
            thriceError("Number ended after the base indicator!");
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
        (thr_str){.beg = word.beg, .end = end},
        src);
}

bool thriceLexerIdChr(const uint8_t chr)
{
    return chr == '_' || thriceLetter(chr);
}

thr_lxd_tkn thriceLex(const thr_str src)
{
    const thr_str word = thriceLexerWord(src);

    if (!thriceStringLength(word)) {
        return thriceLexerCreate(THR_TOKEN_END_OF_FILE, word, src);
    }

    { // Check for punctuation.
        const thr_str  puncs = thriceStringStatic(";()[]{}");
        const uint8_t* pos   = thriceStringFirstPosChr(puncs, *word.beg);

        if (pos < puncs.end) {
            return thriceLexerCreate(
                THR_TOKEN_SEMICOLON + (pos - puncs.beg),
                thriceStringPart(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (thriceDigit(*word.beg)) {
        return thriceLexerNumber(word, src);
    }

    // Check for identifier or keyword.
    if (thriceLexerIdChr(*word.beg)) {
        const uint8_t* end = word.beg + 1;
        while (end < word.end &&
               (thriceLexerIdChr(*end) || thriceDigit(*end))) {
            end++;
        }
        const thr_str val = {.beg = word.beg, .end = end};
#define THR_LEXER_KEYWORD_COUNT 3
        const thr_str keywords[THR_LEXER_KEYWORD_COUNT] = {
            thriceStringStatic("sz"),
            thriceStringStatic("str"),
            thriceStringStatic("return"),
        };
        for (size_t i = 0; i < THR_LEXER_KEYWORD_COUNT; i++) {
            if (thriceStringEquals(keywords[i], val)) {
                return thriceLexerCreate(THR_TOKEN_KEYWORD_SIZE + i, val, src);
            }
        }
        return thriceLexerCreate(THR_TOKEN_IDENTIFIER, val, src);
    }

    const thr_str val = thriceStringPart(word, 0, 1);
    return thriceLexerCreate(-1, val, src);
}

#endif // THR_LEXER
