// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_LEXER
#define THRICE_LEXER

#include "error.c"
#include "string.c"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum tknt {
    THRICE_TOKEN_SEMCLN,
    THRICE_TOKEN_OBRCKT,
    THRICE_TOKEN_CBRCKT,
    THRICE_TOKEN_OSBRKT,
    THRICE_TOKEN_CSBRKT,
    THRICE_TOKEN_OCBRKT,
    THRICE_TOKEN_CCBRKT,
    THRICE_TOKEN_INTEGR,
    THRICE_TOKEN_KSZ,
    THRICE_TOKEN_KSTR,
    THRICE_TOKEN_KRETRN,
    THRICE_TOKEN_ID,
    THRICE_TOKEN_EOF
};

const char* thriceTokenName(const enum tknt typ)
{
    switch (typ) {
        case THRICE_TOKEN_SEMCLN:
            return "semicolon";
        case THRICE_TOKEN_OBRCKT:
            return "opening-bracket";
        case THRICE_TOKEN_CBRCKT:
            return "closing-bracket";
        case THRICE_TOKEN_OSBRKT:
            return "opening-square-bracket";
        case THRICE_TOKEN_CSBRKT:
            return "closing-square-bracket";
        case THRICE_TOKEN_OCBRKT:
            return "opening-curly-bracket";
        case THRICE_TOKEN_CCBRKT:
            return "closing-curly-bracket";
        case THRICE_TOKEN_INTEGR:
            return "integer";
        case THRICE_TOKEN_KSZ:
            return "sz keyword";
        case THRICE_TOKEN_KSTR:
            return "str keyword";
        case THRICE_TOKEN_KRETRN:
            return "return keyword";
        case THRICE_TOKEN_ID:
            return "identifier";
        case THRICE_TOKEN_EOF:
            return "end-of-file";
        default:
            return "unkown-token";
    }
}

struct tkn {
    enum tknt  typ;
    struct str val;
};

void thriceTokenPrint(const struct tkn tkn)
{
    printf("%s", thriceTokenName(tkn.typ));
    switch (tkn.typ) {
        case THRICE_TOKEN_SEMCLN:
        case THRICE_TOKEN_OBRCKT:
        case THRICE_TOKEN_CBRCKT:
        case THRICE_TOKEN_OSBRKT:
        case THRICE_TOKEN_CSBRKT:
        case THRICE_TOKEN_OCBRKT:
        case THRICE_TOKEN_CCBRKT:
        case THRICE_TOKEN_KSZ:
        case THRICE_TOKEN_KSTR:
        case THRICE_TOKEN_KRETRN:
        case THRICE_TOKEN_EOF:
            break;
        case THRICE_TOKEN_INTEGR:
        case THRICE_TOKEN_ID:
        default:
            printf(" {%.*s}", (int)thriceStringLength(tkn.val), tkn.val.beg);
    }
    printf("\n");
}

struct lex {
    struct tkn tkn;
    struct str src;
};

struct str thriceLexerWord(const struct str src)
{
    const struct str trm = thriceStringTrim(src);
    return thriceStringFirstWord(trm);
}

struct lex thriceLexerCreate(
    const enum tknt  typ,
    const struct str val,
    const struct str src)
{
    return (struct lex){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.beg = val.end, .end = src.end}
    };
}

struct lex thriceLexerNumber(const struct str word, const struct str src)
{
    const size_t len = thriceStringLength(word);

    if (len == 1) {
        return thriceLexerCreate(THRICE_TOKEN_INTEGR, word, src);
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
                    THRICE_TOKEN_INTEGR,
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
        THRICE_TOKEN_INTEGR,
        (struct str){.beg = word.beg, .end = end},
        src);
}

bool thriceLexerIdChr(const uint8_t chr)
{
    return chr == '_' || thriceLetter(chr);
}

struct lex thriceLex(const struct str src)
{
    const struct str word = thriceLexerWord(src);

    if (!thriceStringLength(word)) {
        return thriceLexerCreate(THRICE_TOKEN_EOF, word, src);
    }

    { // Check for punctuation.
        const struct str puncs = thriceStringStatic(";()[]{}");
        const uint8_t*   pos   = thriceStringFirstPosChr(puncs, *word.beg);

        if (pos < puncs.end) {
            return thriceLexerCreate(
                THRICE_TOKEN_SEMCLN + (pos - puncs.beg),
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
        const struct str val = {.beg = word.beg, .end = end};
#define THRICE_LEXER_KEYWORD_COUNT 3
        const struct str keywords[THRICE_LEXER_KEYWORD_COUNT] = {
            thriceStringStatic("sz"),
            thriceStringStatic("str"),
            thriceStringStatic("return"),
        };
        for (size_t i = 0; i < THRICE_LEXER_KEYWORD_COUNT; i++) {
            if (thriceStringEquals(keywords[i], val)) {
                return thriceLexerCreate(THRICE_TOKEN_KSZ + i, val, src);
            }
        }
        return thriceLexerCreate(THRICE_TOKEN_ID, val, src);
    }

    const struct str val = thriceStringPart(word, 0, 1);
    return thriceLexerCreate(-1, val, src);
}

#endif // THRICE_LEXER
