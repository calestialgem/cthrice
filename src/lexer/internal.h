// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@ouotlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include <stddef.h>

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

/** Compound token pattern, which is a tree and it is stored as an array of
 * patterns. */
typedef struct Rule Rule;

/** Type of a token pattern. It could a character, a unary pattern, a binary
 * pattern, or a rule, which is just a compound pattern. */
typedef enum {
    PATTERN_CHARACTER,
    PATTERN_UNARY,
    PATTERN_BINARY,
    PATTERN_RULE
} Pattern_Type;

/** Token pattern. */
typedef struct {
    Pattern_Type typ;
    union {
        char           chr;
        Pattern_Unary  unr;
        Pattern_Binary bir;
        Rule*          rle;
    };
} Pattern;

/** Definition of the rule. */
struct Rule {
    const Pattern* bgn;
    const Pattern* end;
};

/** Definition of the lexer. */
struct Cthrice_Lexer {
    const Rule* bgn;
    const Rule* end;
};

#endif // INTERNAL_H
