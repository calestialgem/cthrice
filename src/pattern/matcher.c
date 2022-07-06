// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>
#include <stdlib.h>

/* The consumed character amount that signals no match. */
const ptr NOT_MATCHED = -1;

/* Test starting from the initial state. Returns the amount of characters that
 * were consumed from the input by the path that was accepted by the automaton
 * first. Negative one means none of the states were accepted before all states
 * died. */
static ptr match(
    struct ptrnctx ctx,
    struct states  mainpaths,
    struct states  subpaths,
    struct state   init)
{
    mainpaths = state_put(mainpaths, init);

    while (mainpaths.end - mainpaths.bgn != 0) {
        // Step all the states, and return if matched.
        for (const struct state* i = mainpaths.bgn; i < mainpaths.end; i++) {
            struct decoderes res = decode(ctx, subpaths, *i);
            subpaths             = res.states;
            if (res.matched) {
                return str_size(init.input) - str_size(i->input);
            }
        }
        // Take all the nondead subpaths to the main states.
        mainpaths = state_clear(mainpaths);
        for (const struct state* i = subpaths.bgn; i < subpaths.end; i++) {
            if (!i->dead) {
                mainpaths = state_put(mainpaths, *i);
            }
        }
        subpaths = state_clear(subpaths);
    }

    return NOT_MATCHED;
}

struct str ptrn_match(struct ptrnctx ctx, struct str input)
{
    struct states mainpaths = {0};
    struct states subpaths  = {0};

    for (const struct ptrninfo* i = ctx.info.bgn; i < ctx.info.end; i++) {
        if (i->visible) {
            // Create the initial state.
            struct state init = {
                .input = input,
                .code  = i->code,
                .dead  = false};

            // If matches a visible pattern, return the name.
            if (match(ctx, mainpaths, subpaths, init) != NOT_MATCHED) {
                mainpaths = state_destroy(mainpaths);
                subpaths  = state_destroy(subpaths);
                return i->name;
            }

            // Clear the states to be reused later.
            mainpaths = state_clear(mainpaths);
            subpaths  = state_clear(subpaths);
        }
    }

    // Return empty string if nothing matched.
    mainpaths = state_destroy(mainpaths);
    subpaths  = state_destroy(subpaths);
    return (struct str){0};
}

bool ptrn_check(struct ptrnctx ctx, struct str name, struct str input)
{
    // TODO: Use hashmap to access to the pattern code.
    return false;
}
