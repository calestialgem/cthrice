// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "pattern/api.h"
#include "string/api.h"
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
        /* Represents alternative transition states. */
        DIVERGE,
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
        /* Data of PATTERN type; index of the reffered pattern. */
        ptr indx;
        /* Data of the DIVERGE type; amount of states. */
        ptr amt;
    };
};

/* Pattern information. */
struct ptrninfo {
    /* Identifier. */
    struct str name;
    /* Index of the start of the pattern's code. */
    ptr code;
    /* Whether the pattern matches in the nameless search. */
    bool visible;
};

/* The state of the nondeterministic finite automaton. */
struct state {
    /* Remaining input. */
    struct str input;
    /* Index of the current pattern code. */
    ptr code;
    /* Whether the last transition failed. */
    bool dead;
};

/* Dynamic array of states. */
struct states {
    /* Pointer to the first allocated state. */
    struct state* bgn;
    /* Pointer to the state after the last valid one. */
    struct state* end;
    /* Pointer to the state after the last allocated one. */
    struct state* lst;
};

/* Result of decoding a state. */
struct decoderes {
    /* Paths after decoding. */
    struct states states;
    /* Whether the state was accepted by the automaton. */
    bool matched;
};

/* Print the pattern information in the context. */
void print_infos(struct ptrnctx);
/* Print the pattern codes in the context. */
void print_codes(struct ptrnctx);
/* Print the pattern code with its type and data. */
void print_code(struct ptrncode);

/* Transition to the next state. */
struct state state_step(struct ptrnctx, struct state);
/* Put the state in to the states. */
struct states state_put(struct states, struct state);
/* Remove all the states. */
struct states state_clear(struct states);
/* Free the allocated memory. */
struct states state_destroy(struct states);

/* Returns the amount of pattern code in the context. */
ptr code_size(struct ptrnctx);
/* Decode the state and add the next states. */
struct decoderes decode(struct ptrnctx, struct states, struct state);

#endif // INTERNAL_H
