// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>
#include <stdlib.h>

/* The state of the nondeterministic finite automaton. */
struct state {
    /* Remaining input. */
    struct str input;
    /* Index of the current pattern code. */
    ptr code;
    /* Whether the last transition failed. */
    bool dead;
};

/* Returns the amount of pattern code in the context. */
static ptr code_size(struct ptrnctx ctx)
{
    ASSERT(ctx.code.end >= ctx.code.bgn, "Negative code size!");
    return ctx.code.end - ctx.code.bgn;
}

/* Transition to the next state. */
static struct state step(struct ptrnctx ctx, struct state state)
{
    ASSERT(state.dead == false, "Stepping dead state!");
    ASSERT(state.code >= 0, "Code out of bounds!");
    ASSERT(state.code < code_size(ctx), "Code out of bounds!");
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
            print_code(code);
            ASSERT(false, "Unexpected type!");
        default:
            // DEBUG: Print the unknown code.
            print_code(code);
            ASSERT(false, "Unknown type!");
    }

    // Move to the target code.
    state.code += code.move;
    ASSERT(state.code >= 0, "Movement out of bounds!");
    ASSERT(state.code < code_size(ctx), "Movement out of bounds!");

    return state;
}

/* Dynamic array of all alive states. */
struct paths {
    /* Pointer to the first allocated state. */
    struct state* bgn;
    /* Pointer to the state after the last valid one. */
    struct state* end;
    /* Pointer to the state after the last allocated one. */
    struct state* lst;
};

/* Put the alternative state in to the paths. */
static struct paths path_put(struct paths paths, struct state state)
{
    // Grow if the current avalible space is not enough.
    if (paths.lst - paths.end < 1) {
        // Store the current size to calculate the end pointer later.
        ptr sze = paths.end - paths.bgn;

        // Double the capacity.
        ptr cap = paths.lst - paths.bgn;
        ptr nwc = cap << 1;

        struct state* mem = realloc(paths.bgn, nwc);
        CHECK(mem != null, "Cannot allocate paths!");

        paths.bgn = mem;
        paths.end = mem + sze;
        paths.lst = mem + nwc;
    }

    *paths.end++ = state;
    return paths;
}

/* Free the allocated memory. */
static struct paths path_destroy(struct paths paths)
{
    free(paths.bgn);
    paths.bgn = paths.end = paths.lst = null;
    return paths;
}

/* Result of stepping all the states. */
struct stepres {
    /* Paths after stepping. */
    struct paths paths;
    /* Whether any state was accepted by the automaton. */
    bool matched;
};

/* Transition the states in all the paths. Returns early when a path is accepted
 * by the automaton. */
static struct stepres
step_all(struct ptrnctx ctx, struct paths paths, struct state state)
{
    ASSERT(state.code >= 0, "Code out of bounds!");
    ASSERT(state.code < code_size(ctx), "Code out of bounds!");
    struct ptrncode code = *(ctx.code.bgn + state.code);

    switch (code.type) {
        case DIVERGE:
            ASSERT(code.amt > 0, "Nonpositive amount!");
            ASSERT(
                state.code + code.amt < code_size(ctx),
                "Divergence out of bounds!");
            for (ptr j = 0; j < code.amt; j++) {
                state.code++;
                paths = path_put(paths, step(ctx, state));
            }
            break;
        case MATCH:
            return (struct stepres){.paths = paths, .matched = true};
        default:
            paths = path_put(paths, step(ctx, state));
    }

    return (struct stepres){.paths = paths, .matched = false};
}

/* The consumed character amount that signals no match. */
const ptr NOT_MATCHED = -1;

/* Test starting from the initial state. Returns the amount of characters that
 * were consumed from the input by the path that was accepted by the automaton
 * first. Negative one means none of the paths were accepted before all paths
 * died. */
static ptr match(struct ptrnctx ctx, struct state init)
{
    struct paths paths = {0};
    paths              = path_put(paths, init);

    while (paths.end - paths.bgn != 0) {
        for (ptr i = 0; i < paths.end - paths.bgn; i++) {
            // Remove the dead ends.
            struct state state = paths.bgn[i];
            if (state.dead) {
                paths.bgn[i--] = *--paths.end;
                continue;
            }
            // Step all the states, and return if matched.
            struct stepres res = step_all(ctx, paths, state);
            if (res.matched) {
                ptr consumed = str_size(init.input) - str_size(state.input);
                path_destroy(paths);
                return consumed;
            }
            paths = res.paths;

            // Remove the old state.
            paths.bgn[i--] = *--paths.end;
        }
    }

    path_destroy(paths);
    return NOT_MATCHED;
}

struct str ptrn_match(struct ptrnctx ctx, struct str input)
{
    for (const struct ptrninfo* i = ctx.info.bgn; i < ctx.info.end; i++) {
        if (i->visible) {
            // Create the initial state.
            struct state init = {
                .input = input,
                .code  = i->code,
                .dead  = false};

            // If matches a visible pattern, return the name.
            if (match(ctx, init) != NOT_MATCHED) {
                return i->name;
            }
        }
    }

    // Return empty string if nothing matched.
    return (struct str){0};
}

bool ptrn_check(struct ptrnctx ctx, struct str name, struct str input)
{
    // TODO: Use hashmap to access to the pattern code.
    return false;
}
