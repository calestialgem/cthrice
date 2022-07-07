// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"

#include <stdlib.h>

struct ptrnstate ptrn_state_step(struct ptrnctx ctx, struct ptrnstate state)
{
    ASSERT(state.dead == false, "Stepping dead state!");
    ASSERT(state.code >= 0, "Code out of bounds!");
    ASSERT(state.code < ptrn_code_size(ctx), "Code out of bounds!");
    struct ptrncode code = *(ctx.code.bgn + state.code);

    switch (code.type) {
        case EMPTY:
            break;
        case LITERAL:
            // Check the next input and consume it.
            state.dead =
                !str_finite(state.input) || *state.input.bgn++ != code.ltrl;
            break;
        case RANGE:
            // Check the next input and consume it.
            state.dead =
                !str_finite(state.input) || !(*state.input.bgn++ >= code.bgn &&
                                              *state.input.bgn <= code.end);
            break;
        case PATTERN:
            // TODO: Implement referencing other patterns.
            ASSERT(false, "Not implemented!");
        case DIVERGE:
        case MATCH:
            // DEBUG: Print the unexpected code.
            ptrn_print_code(code);
            ASSERT(false, "Unexpected type!");
        default:
            // DEBUG: Print the unknown code.
            ptrn_print_code(code);
            ASSERT(false, "Unknown type!");
    }

    // Move to the target code.
    state.code += code.move;
    ASSERT(state.code >= 0, "Movement out of bounds!");
    ASSERT(state.code < ptrn_code_size(ctx), "Movement out of bounds!");

    return state;
}

struct ptrnstates
ptrn_state_put(struct ptrnstates states, struct ptrnstate state)
{
    // Grow if the current avalible space is not enough.
    if (states.lst - states.end < 1) {
        // Store the current size to calculate the end pointer later.
        ptr sze = states.end - states.bgn;

        // Double the capacity.
        ptr cap = states.lst - states.bgn;
        ptr nwc = cap << 1;

        struct ptrnstate* mem = realloc(states.bgn, nwc);
        CHECK(mem != null, "Cannot allocate states!");

        states.bgn = mem;
        states.end = mem + sze;
        states.lst = mem + nwc;
    }

    *states.end++ = state;
    return states;
}

struct ptrnstates ptrn_state_clear(struct ptrnstates states)
{
    states.end = states.bgn;
    return states;
}

struct ptrnstates ptrn_state_destroy(struct ptrnstates states)
{
    free(states.bgn);
    states.bgn = states.end = states.lst = null;
    return states;
}
