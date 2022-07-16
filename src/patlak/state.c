// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

#include <stdbool.h>

/* State of the nondeterministic finite automaton. */
typedef struct {
    /* Remaining input. */
    CTString input;
    /* Index of the code. */
    CTIndex code;
    /* Whether the state reached a dead end. */
    bool dead;
} CTPatlakState;

/* Dynamic array of states. */
typedef struct {
    /* Border before the first state. */
    CTPatlakState* first;
    /* Border after the last state. */
    CTPatlakState* last;
    /* Border after the last allocated state. */
    CTPatlakState* allocated;
} CTPatlakStates;

/* Amount of states. */
CTIndex ct_patlak_states_size(CTPatlakStates const* states)
{
    return states->last - states->first;
}

/* Amount of allocated states. */
CTIndex ct_patlak_states_capacity(CTPatlakStates const* states)
{
    return states->allocated - states->first;
}

/* Amount of allocated but unused states. */
CTIndex ct_patlak_states_space(CTPatlakStates const* states)
{
    return states->allocated - states->last;
}

/* Make sure the amount of states will fit. Grows by at least the half of
 * the current capacity if necessary. */
void ct_patlak_states_reserve(CTPatlakStates* states, CTIndex amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_patlak_states_space(states);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_patlak_states_capacity(states);
    CTIndex half_capacity = capacity >> 1;
    if (growth < half_capacity) {
        growth = half_capacity;
    }

    CTIndex        new_capacity = capacity + growth;
    CTPatlakState* memory =
        reallocarray(states->first, new_capacity, sizeof(CTPatlakState));
    ct_expect(memory != NULL, "Could not allocate!");

    states->last      = memory + ct_patlak_states_size(states);
    states->first     = memory;
    states->allocated = memory + new_capacity;
}

/* Add to the end of states. */
void ct_patlak_states_add(CTPatlakStates* states, CTPatlakState state)
{
    ct_patlak_states_reserve(states, 1);
    *states->last++ = state;
}

/* Deallocate memory. */
void ct_patlak_states_free(CTPatlakStates* states)
{
    free(states->first);
    states->first     = NULL;
    states->last      = NULL;
    states->allocated = NULL;
}
