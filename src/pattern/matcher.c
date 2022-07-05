// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>

/* The state of the nondeterministic finite automaton. */
struct state {
    /* Remaining input. */
    struct str input;
    /* Index of the current pattern code. */
    ptr code;
    /* Whether the last transition failed. */
    bool dead;
};

/* Dynamic array of all alive states. */
struct paths {
    /* Pointer to the first allocated state. */
    struct state* bgn;
    /* Pointer to the state after the last valid one. */
    struct state* end;
    /* Pointer to the state after the last allocated one. */
    struct state* lst;
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

/* Put the alternative state in to the paths. */
static struct paths put(struct paths paths, struct state state)
{
    CHECK(paths.lst - paths.end > 0, "Out of space!");
    *paths.end++ = state;
    return paths;
}

struct stepres {
    struct paths paths;
    bool         matched;
};

/* Transition the states in all the paths. */
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
                paths = put(paths, step(ctx, state));
            }
            break;
        case MATCH:
            return (struct stepres){.paths = paths, .matched = true};
        default:
            paths = put(paths, step(ctx, state));
    }

    return (struct stepres){.paths = paths, .matched = false};
}

/* Test the given paths. */
static bool match(struct ptrnctx ctx, struct paths paths)
{
    while (paths.end - paths.bgn != 0) {
        for (ptr i = 0; i < paths.end - paths.bgn; i++) {
            // Remove the dead ends.
            struct state path = paths.bgn[i];
            if (path.dead) {
                paths.bgn[i--] = *--paths.end;
                continue;
            }
            // Step all the states, and return if matched.
            struct stepres res = step_all(ctx, paths, path);
            if (res.matched) {
                return true;
            }
            paths = res.paths;

            // Remove the old state.
            paths.bgn[i--] = *--paths.end;
        }
    }

    return false;
}

struct str ptrn_match(struct ptrnctx ctx, struct str input)
{
    for (const struct ptrninfo* i = ctx.info.bgn; i < ctx.info.end; i++) {
        if (i->visible) {
            // Use stack memory for paths.
#define PATH_CAPACITY 32
            struct state memory[PATH_CAPACITY];
            struct paths paths = {
                .bgn = memory,
                .end = memory,
                .lst = memory + PATH_CAPACITY};

            // Put the initial state.
            struct state init = {
                .input = input,
                .code  = i->code,
                .dead  = false};
            paths = put(paths, init);

            // If matches a visible pattern, return the name.
            if (match(ctx, paths)) {
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
