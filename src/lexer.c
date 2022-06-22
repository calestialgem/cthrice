// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_LEXER
#define THRICE_LEXER

#include "error.c"
#include "string.c"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
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
} ThriceTokenType;

const char* ThriceTokenName(const ThriceTokenType typ)
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

typedef struct {
    ThriceTokenType typ;
    ThriceString    val;
} ThriceToken;

void ThriceTokenPrint(const ThriceToken tkn)
{
    printf("%s", ThriceTokenName(tkn.typ));
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
            printf(" {%.*s}", (int)ThriceStringLength(tkn.val), tkn.val.beg);
    }
    printf("\n");
}

typedef struct {
    ThriceToken  tkn;
    ThriceString src;
} ThriceLexedToken;

ThriceString ThriceLexerWord(const ThriceString src)
{
    const ThriceString trm = ThriceStringTrim(src);
    return ThriceStringFirstWord(trm);
}

ThriceLexedToken ThriceLexerCreate(
    const ThriceTokenType typ,
    const ThriceString    val,
    const ThriceString    src)
{
    return (ThriceLexedToken){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.beg = val.end, .end = src.end}
    };
}

ThriceLexedToken
ThriceLexerNumber(const ThriceString word, const ThriceString src)
{
    const size_t len = ThriceStringLength(word);

    if (len == 1) {
        return ThriceLexerCreate(THRICE_TOKEN_INTEGR, word, src);
    }

    bool (*digit)(uint8_t) = &ThriceDigit;
    const uint8_t* end     = word.beg + 1;

    if ('0' == *word.beg) {
        switch (*end) {
            case 'b':
                digit = &ThriceDigitBin;
                break;
            case 'o':
                digit = &ThriceDigitOct;
                break;
            case 'x':
                digit = &ThriceDigitHex;
                break;
            case '_':
                break;
            default:
                return ThriceLexerCreate(
                    THRICE_TOKEN_INTEGR,
                    ThriceStringPart(word, 0, 1),
                    src);
        }

        if (++end >= word.end) {
            ThriceError("Number ended after the base indicator!");
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

    return ThriceLexerCreate(
        THRICE_TOKEN_INTEGR,
        (ThriceString){.beg = word.beg, .end = end},
        src);
}

bool ThriceLexerIdChr(const uint8_t chr)
{
    return chr == '_' || ThriceLetter(chr);
}

ThriceLexedToken ThriceLex(const ThriceString src)
{
    const ThriceString word = ThriceLexerWord(src);

    if (!ThriceStringLength(word)) {
        return ThriceLexerCreate(THRICE_TOKEN_EOF, word, src);
    }

    { // Check for punctuation.
        const ThriceString puncs = ThriceStringStatic(";()[]{}");
        const uint8_t*     pos   = ThriceStringFirstPosChr(puncs, *word.beg);

        if (pos < puncs.end) {
            return ThriceLexerCreate(
                THRICE_TOKEN_SEMCLN + (pos - puncs.beg),
                ThriceStringPart(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (ThriceDigit(*word.beg)) {
        return ThriceLexerNumber(word, src);
    }

    // Check for identifier or keyword.
    if (ThriceLexerIdChr(*word.beg)) {
        const uint8_t* end = word.beg + 1;
        while (end < word.end &&
               (ThriceLexerIdChr(*end) || ThriceDigit(*end))) {
            end++;
        }
        const ThriceString val = {.beg = word.beg, .end = end};
#define THRICE_LEXER_KEYWORD_COUNT 3
        const ThriceString keywords[THRICE_LEXER_KEYWORD_COUNT] = {
            ThriceStringStatic("sz"),
            ThriceStringStatic("str"),
            ThriceStringStatic("return"),
        };
        for (size_t i = 0; i < THRICE_LEXER_KEYWORD_COUNT; i++) {
            if (ThriceStringEquals(keywords[i], val)) {
                return ThriceLexerCreate(THRICE_TOKEN_KSZ + i, val, src);
            }
        }
        return ThriceLexerCreate(THRICE_TOKEN_ID, val, src);
    }

    const ThriceString val = ThriceStringPart(word, 0, 1);
    return ThriceLexerCreate(-1, val, src);
}

#endif // THRICE_LEXER
