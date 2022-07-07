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
};

/* The state of the nondeterministic finite automaton. */
struct ptrnstate {
    /* Remaining input. */
    struct str input;
    /* Index of the current pattern code. */
    ptr code;
    /* Whether the last transition failed. */
    bool dead;
};

/* Dynamic array of states. */
struct ptrnstates {
    /* Pointer to the first allocated state. */
    struct ptrnstate* bgn;
    /* Pointer to the state after the last valid one. */
    struct ptrnstate* end;
    /* Pointer to the state after the last allocated one. */
    struct ptrnstate* lst;
};

/* Result of decoding a state. */
struct ptrndecoderes {
    /* States after decoding. */
    struct ptrnstates next;
    /* Whether the state was accepted by the automaton. */
    bool matched;
};

/* Print the pattern information in the context. */
void ptrn_print_infos(struct ptrnctx);
/* Print the pattern codes in the context. */
void ptrn_print_codes(struct ptrnctx);
/* Print the pattern code with its type and data. */
void ptrn_print_code(struct ptrncode);

/* Put the state in to the states. */
struct ptrnstates ptrn_state_put(struct ptrnstates, struct ptrnstate);
/* Remove all the states. */
struct ptrnstates ptrn_state_clear(struct ptrnstates);
/* Free the allocated memory. */
struct ptrnstates ptrn_state_destroy(struct ptrnstates);

/* Returns the amount of pattern code in the context. */
ptr ptrn_code_size(struct ptrnctx);
/* Decode the state and add the next states. */
struct ptrndecoderes
    ptrn_decode(struct ptrnctx, struct ptrnstates, struct ptrnstate);
/* Decode until the end starting from the initial state. Returns the initial
 * portion of the input that was accepted by the automaton first. Empty match
 * means none of the states were accepted before all states died. */
struct str ptrn_decode_all(struct ptrnctx, struct ptrnstate);

#endif // INTERNAL_H
