// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "pattern/api.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>

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

/* Print the pattern information. */
static void print_patterns(struct ptrns ptrns)
{
    printf("\n  PATTERNS\n============\n\n");
    for (const struct ptrn* i = ptrns.values.bgn; i < ptrns.values.end; i++) {
        printf(
            "Pattern {%.*s} [%05lld]\n",
            (int)str_size(i->name),
            i->name.bgn,
            i->code);
    }
}

/* Print the code with its type and data. */
static void print_code(struct ptrncode code)
{
    switch (code.type) {
        case EMPTY:
            printf("EMPTY");
            break;
        case LITERAL:
            printf("LITERAL {%c}", code.ltrl);
            break;
        case RANGE:
            printf("RANGE {%c~%c}", code.bgn, code.end);
            break;
        case PATTERN:
            printf("PATTERN {%05lld}", code.off);
            break;
        case MATCH:
            printf("MATCH");
            break;
    }
    printf(" %+lld\n", code.move);
}

/* Print the codes in the pattern. */
static void print_codes(struct ptrns ptrns)
{
    printf("\n  PATTERN CODE\n================\n\n");
    for (const struct ptrncode* i = ptrns.code.bgn; i < ptrns.code.end; i++) {
        printf("[%05lld] ", i - ptrns.code.bgn);
        print_code(*i);
    }
}

#endif // INTERNAL_H
