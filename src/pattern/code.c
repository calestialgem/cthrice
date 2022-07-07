// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"

/* Result of decoding a state. */
struct result {
    /* States after decoding. */
    struct ptrnstates next;
    /* Whether the state was accepted by the automaton. */
    bool matched;
};

/* Returns the amount of pattern code in the context. */
static ptr size(struct ptrnctx ctx)
{
    ASSERT(ctx.code.end >= ctx.code.bgn, "Negative code size!");
    return ctx.code.end - ctx.code.bgn;
}

/* Decode the state and add the next states. */
static struct result
decode(struct ptrnctx ctx, struct ptrnstates next, struct ptrnstate state)
{
    ASSERT(state.dead == false, "Decoding a dead state!");
    ASSERT(state.code >= 0, "Code out of bounds!");
    ASSERT(state.code < size(ctx), "Code out of bounds!");
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
            state.dead = !str_finite(state.input) ||
                       *state.input.bgn++ < code.bgn ||
                       *state.input.bgn > code.end;
            break;
        case PATTERN:
            // Check the reffered pattern.
            struct ptrnstate ref = {
                .input = state.input,
                .code  = code.indx,
                .dead  = false};
            struct str match = ptrn_decode(ctx, ref);
            // Consume the input.
            ASSERT(
                match.end <= state.input.end,
                "Matched longer than the input!");
            state.input.bgn = match.end;
            state.dead      = !str_finite(match);
        case DIVERGE:
            ASSERT(code.amt > 0, "Nonpositive amount!");
            ASSERT(
                state.code + code.amt < size(ctx),
                "Divergence out of bounds!");
            // Add all diverging states to the next states.
            for (ptr j = 0; j < code.amt; j++) {
                state.code++;
                next = ptrn_state_put(next, state);
            }
            goto end;
        case MATCH:
            return (struct result){.next = next, .matched = true};
        default:
            // DEBUG: Print the unknown code.
            ptrn_print_code(code);
            ASSERT(false, "Unknown type!");
    }

    // Add the target state to the next states..
    state.code += code.move;
    ASSERT(state.code >= 0, "Movement out of bounds!");
    ASSERT(state.code < size(ctx), "Movement out of bounds!");
    next = ptrn_state_put(next, state);

end:
    return (struct result){.next = next, .matched = false};
}

struct str ptrn_decode(struct ptrnctx ctx, struct ptrnstate init)
{
    struct str        match  = {0};
    struct ptrnstates active = {0};
    struct ptrnstates next   = {0};

    // Put the initial state.
    ASSERT(!init.dead, "Initial state is dead!");
    active = ptrn_state_put(active, init);

    // Until all states die.
    while (active.end - active.bgn != 0) {
        // Step all the active states and collect all the next states
        next = ptrn_state_clear(next);
        for (const struct ptrnstate* i = active.bgn; i < active.end; i++) {
            struct result res = decode(ctx, next, *i);
            next              = res.next;

            // Return early if matched.
            if (res.matched) {
                match.bgn = init.input.bgn;
                match.end = i->input.bgn;
                ASSERT(str_finite(match), "Did not consume anything!");
                ASSERT(str_size(match) > 0, "Consumed more than the input!");
                goto end;
            }
        }

        // Take all the nondead next states to the active states.
        active = ptrn_state_clear(active);
        for (const struct ptrnstate* i = next.bgn; i < next.end; i++) {
            if (!i->dead) {
                active = ptrn_state_put(active, *i);
            }
        }
    }

end:
    active = ptrn_state_destroy(active);
    next   = ptrn_state_destroy(next);
    return match;
}
