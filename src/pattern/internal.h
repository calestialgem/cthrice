// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "types/api.h"

/** Implementation of the pattern code. These are the transitions in a
 * nondeterministic finite automaton with empty moves. */
struct ptrncode {
    /* The amount of pattern code to move forward. Zero means a match. */
    ptr move;
    /* Type of the pattern code. */
    enum {
        /* Move regardless without consuming a character. */
        EMPTY,
        /* Move if the character equals to the literal. */
        LITERAL,
        /* Move if the character is in the range. */
        RANGE,
        /* Move if the reffered pattern matches. */
        PATTERN,
        /* Pattern matches if it reached here. */
        MATCH
    } type;
    /* Data of the pattern code. */
    union {
        /* Data of LITERAL type; a single literal to match. */
        byte ltrl;
        /* Data of RANGE type. */
        struct {
            /* Beginin of the range. */
            byte bgn;
            /* End of the range. */
            byte end;
        };
        /* Data of PATTERN type; offset of the reffered pattern. */
        ptr off;
    };
};

#endif // INTERNAL_H
