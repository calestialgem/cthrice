// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "patlak/state.c"
#include "prelude/expect.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

#include <stdbool.h>

// Prototype for call before definition.
CTString ct_patlak_decode_test(CTPatlakCodes const* codes, CTPatlakState init);

/* Decode the state using the codes and add the states come after it to
 * the next states. */
bool ct_patlak_decode(
    CTPatlakCodes const* codes,
    CTPatlakStates*      next,
    CTPatlakState        state)
{
    ct_expect(!state.dead, "Decoding a dead state!");
    CTPatlakCode const* code = ct_patlak_codes_get(codes, state.code);

    switch (code->type) {
        case CT_PATLAK_CODE_EMPTY:
            break;
        case CT_PATLAK_CODE_LITERAL:
            // Check the next input and consume it.
            state.dead = !ct_string_finite(&state.input) ||
                       *state.input.first++ != code->literal;
            break;
        case CT_PATLAK_CODE_RANGE:
            // Check the next input and consume it.
            state.dead = !ct_string_finite(&state.input) ||
                       *state.input.first++ < code->first ||
                       *state.input.first > code->last;
            break;
        case CT_PATLAK_CODE_REFERANCE: {
            // Check the reffered pattern.
            CTPatlakState ref = {
                .input = state.input,
                .code  = code->reffered,
                .dead  = false};
            CTString match = ct_patlak_decode_test(codes, ref);
            // Consume the input.
            state.input.first = match.last;
            state.dead        = !ct_string_finite(&match);
        } break;
        case CT_PATLAK_CODE_BRANCH:
            ct_expect(code->branches > 0, "Nonpositive branch amount!");
            ct_expect(
                ct_patlak_codes_valid(codes, state.code + code->branches),
                "Branching out of bounds!");
            // Add all diverging states to the next states.
            for (CTIndex j = 0; j < code->branches; j++) {
                state.code++;
                ct_patlak_states_add(next, state);
            }
            goto end;
        case CT_PATLAK_CODE_TERMINAL:
            return true;
        default:
            ct_expect(false, "Unkown type!");
    }

    // Add the target state to the next states.
    state.code += code->movement;
    ct_expect(
        ct_patlak_codes_valid(codes, state.code),
        "Movement out of bounds!");
    ct_patlak_states_add(next, state);

end:
    return false;
}

/* Decode until the end starting from the initial state. Returns the
 * initial portion of the input that was accepted by the
 * nondeterministic finite automaton first. Empty match means none of
 * the states were accepted before all states died. */
CTString ct_patlak_decode_test(CTPatlakCodes const* codes, CTPatlakState init)
{
    CTString       match  = {0};
    CTPatlakStates active = {0};
    CTPatlakStates next   = {0};

    // Put the initial state.
    ct_expect(!init.dead, "Initial state is dead!");
    ct_patlak_states_add(&active, init);

    // Until all states die.
    while (ct_patlak_states_finite(&active)) {
        // Step all the active states and collect all the next states.
        ct_patlak_states_clear(&next);
        for (const CTPatlakState* i = active.first; i < active.last; i++) {
            bool matched = ct_patlak_decode(codes, &next, *i);

            // Return early if matched.
            if (matched) {
                match.first = init.input.first;
                match.last  = i->input.first;
                ct_expect(
                    ct_string_finite(&match),
                    "Did not consume anything!");
                goto end;
            }
        }

        // Take all the nondead next states to the active states.
        ct_patlak_states_clear(&active);
        for (const CTPatlakState* i = next.first; i < next.last; i++) {
            if (!i->dead) {
                ct_patlak_states_add(&active, *i);
            }
        }
    }

end:
    ct_patlak_states_free(&active);
    ct_patlak_states_free(&next);
    return match;
}
