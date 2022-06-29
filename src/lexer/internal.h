// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "string/mod.h"

#include <stddef.h>
#include <stdint.h>

/** Token pattern over a single character. */
typedef enum {
    UNARY_OPTIONAL,
    UNARY_ZERO_OR_MORE,
    UNARY_ONE_OR_MORE,
} Pattern_Unary;

/** Token pattern over two characters. */
typedef enum {
    BINARY_RANGE,
    BINARY_AND,
    BINARY_OR,
} Pattern_Binary;

/** Function call in the pattern. */
typedef struct {
    Cthrice_String nme;
    uint8_t        arg;
} Pattern_Function;

/** Compound token pattern, which is a tree and it is stored as an array of
 * patterns. */
typedef struct Rule Rule;

/** Type of a token pattern. It could be a character, a unary pattern, a binary
 * pattern, a call to another function, a parameter to the current function, or
 * a rule, which is just a compound pattern. */
typedef enum {
    PATTERN_CHARACTER,
    PATTERN_UNARY,
    PATTERN_BINARY,
    PATTERN_FUNCTION,
    PATTERN_RULE
} Pattern_Type;

/** Token pattern. */
typedef struct {
    Pattern_Type typ;
    union {
        char             chr;
        Pattern_Unary    unr;
        Pattern_Binary   bir;
        Pattern_Function fnc;
        Cthrice_String   rle;
    };
} Pattern;

/** Definition of the rule. */
struct Rule {
    Cthrice_String nme;
    const Pattern* bgn;
    const Pattern* end;
};

/** Definition of the lexer. */
struct Cthrice_Lexer {
    const Rule* bgn;
    const Rule* end;
};

#endif // INTERNAL_H
