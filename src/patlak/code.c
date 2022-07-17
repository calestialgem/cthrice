// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.c"
#include "prelude/scalar.c"

#include <stdlib.h>

/* Compiled pattern information. These are the transitions in the
 * nondeterministic finite automaton with empty moves. */
typedef struct {
    /* Amount of code to move forward. Zero means a match. */
    CTIndex movement;
    /* Type. */
    enum {
        /* Move regardless without consuming a character. */
        CT_PATLAK_CODE_EMPTY,
        /* Move if the character equals to the literal. */
        CT_PATLAK_CODE_LITERAL,
        /* Move if the character is in the range. */
        CT_PATLAK_CODE_RANGE,
        /* Move if the reffered pattern matches. */
        CT_PATLAK_CODE_REFERANCE,
        /* Divergence of alternative transition states. */
        CT_PATLAK_CODE_BRANCH,
        /* End of a pattern, meaning a match. */
        CT_PATLAK_CODE_TERMINAL
    } type;
    /* Data. */
    union {
        /* Data of LITER AL type. A single literal to match. */
        char literal;
        /* Data of RANGE type. */
        struct {
            /* First character in the range. */
            char first;
            /* Last character in the range. */
            char last;
        };
        /* Data of REFERANCE type. Index of the reffered pattern. */
        CTIndex reffered;
        /* Data of BRANCH type. Amount of branches. */
        CTIndex branches;
    };
} CTPatlakCode;

/* Dynamic array of codes. */
typedef struct {
    /* Border before the first code. */
    CTPatlakCode* first;
    /* Border after the last code. */
    CTPatlakCode* last;
    /* Border after the last allocated code. */
    CTPatlakCode* allocated;
} CTPatlakCodes;

/* Amount of codes. */
CTIndex ct_patlak_codes_size(CTPatlakCodes const* codes)
{
    return codes->last - codes->first;
}

/* Amount of allocated codes. */
CTIndex ct_patlak_codes_capacity(CTPatlakCodes const* codes)
{
    return codes->allocated - codes->first;
}

/* Amount of allocated but unused codes. */
CTIndex ct_patlak_codes_space(CTPatlakCodes const* codes)
{
    return codes->allocated - codes->last;
}

/* Whether the index is valid. */
bool ct_patlak_codes_valid(CTPatlakCodes const* codes, CTIndex index)
{
    return index >= 0 && index < ct_patlak_codes_size(codes);
}

/* Pointer to the code at the index. */
CTPatlakCode* ct_patlak_codes_get(CTPatlakCodes const* codes, CTIndex index)
{
    ct_expect(ct_patlak_codes_valid(codes, index), "Index out of bounds!");
    return codes->first + index;
}

/* Make sure the amount of codes will fit. Grows by at least the half of
 * the current capacity if necessary. */
void ct_patlak_codes_reserve(CTPatlakCodes* codes, CTIndex amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_patlak_codes_space(codes);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_patlak_codes_capacity(codes);
    CTIndex half_capacity = capacity >> 1;
    if (growth < half_capacity) {
        growth = half_capacity;
    }

    CTIndex       new_capacity = capacity + growth;
    CTPatlakCode* memory =
        reallocarray(codes->first, new_capacity, sizeof(CTPatlakCode));
    ct_expect(memory != NULL, "Could not allocate!");

    codes->last      = memory + ct_patlak_codes_size(codes);
    codes->first     = memory;
    codes->allocated = memory + new_capacity;
}

/* Deallocate memory. */
void ct_patlak_codes_free(CTPatlakCodes* codes)
{
    free(codes->first);
    codes->first     = NULL;
    codes->last      = NULL;
    codes->allocated = NULL;
}
