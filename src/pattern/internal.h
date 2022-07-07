// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "api.h"
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

/* Patern token types. */
enum ptrntkntyp {
    TOKEN_EQUAL,
    TOKEN_DOT,
    TOKEN_PIPE,
    TOKEN_COMMA,
    TOKEN_QUESTION_MARK,
    TOKEN_STAR,
    TOKEN_PLUS,
    TOKEN_OPENING_CURLY_BRACKET,
    TOKEN_CLOSING_CURLY_BRACKET,
    TOKEN_OPENING_SQUARE_BRACKET,
    TOKEN_CLOSING_SQUARE_BRACKET,
    TOKEN_NUMBER,
    TOKEN_QUOTE,
    TOKEN_IDENTIFIER
};

/* Pattern tokens. */
struct ptrntkn {
    enum ptrntkntyp typ;
    struct str      val;
};

/* List of pattern tokens. */
struct ptrnlex {
    /* Pointer to the token at the begining of the allocated memory. */
    struct ptrntkn* bgn;
    /* Pointer to the token one after the end of the valid tokens. */
    struct ptrntkn* end;
    /* Pointer to the token one after the end of the allocated memory. */
    struct ptrntkn* lst;
};

/* Lex the given string. */
struct ptrnlex ptrn_lex(struct str);
/* Free allocated memory. */
struct ptrnlex ptrn_lex_destroy(struct ptrnlex);

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

/* Code index that signals that a pattern with the name does not exist in the
 * hash map. */
extern const ptr PTRN_INVALID_NAME;

/* Find the pattern code from the hash of the name. */
ptr ptrn_hash_get(struct ptrnctx, struct str);
/* Put the pattern info to the hash map. */
struct ptrnctx ptrn_hash_put(struct ptrnctx, struct ptrninfo);
/* Free the allocated memory. */
struct ptrnctx ptrn_hash_destroy(struct ptrnctx);

/* Returns the amount of pattern code in the context. */
ptr ptrn_code_size(struct ptrnctx);
/* Put the code into the list. */
struct ptrnctx ptrn_code_put(struct ptrnctx, struct ptrncode);
/* Free the allocated memory. */
struct ptrnctx ptrn_code_destroy(struct ptrnctx);

/* Print the pattern lexer result. */
void ptrn_print_lex(struct ptrnlex);
/* Print the pattern information in the context. */
void ptrn_print_infos(struct ptrnctx);
/* Print the pattern codes in the context. */
void ptrn_print_codes(struct ptrnctx);
/* Print the pattern code with its type and data. */
void ptrn_print_code(struct ptrncode);

/* Put the state in to the list. */
struct ptrnstates ptrn_state_put(struct ptrnstates, struct ptrnstate);
/* Remove all the states. */
struct ptrnstates ptrn_state_clear(struct ptrnstates);
/* Free the allocated memory. */
struct ptrnstates ptrn_state_destroy(struct ptrnstates);

/* Decode until the end starting from the initial state. Returns the initial
 * portion of the input that was accepted by the automaton first. Empty match
 * means none of the states were accepted before all states died. */
struct str ptrn_decode(struct ptrnctx, struct ptrnstate);

#endif // INTERNAL_H
