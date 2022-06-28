// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@ouotlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "cthrice_types.h"

/** Token rule node over a single character. */
typedef enum {
    UNARY_OPTIONAL,
    UNARY_ZERO_OR_MORE,
    UNARY_ONE_OR_MORE,
    UNARY_TOKEN
} Node_Unary;

/** Token rule node over two characters. */
typedef enum {
    BINARY_RANGE,
    BINARY_AND,
    BINARY_OR,
} Node_Binary;

/** Type of a token rule node. */
typedef enum {
    NODE_UNARY,
    NODE_BINARY,
    NODE_CHARACTER,
    NODE_RULE
} Node_Type;

/** Node in the rule tree that can hold a unary rule, a binary rule, a character
 * or another rule. */
typedef struct {
    Node_Type typ;
    union {
        Node_Unary  unr;
        Node_Binary bir;
        uchr        chr;
        uptr        rle;
    };
} Node;

#endif // INTERNAL_H
