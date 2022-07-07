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
    struct ptrnctx    ctx,
    struct ptrnstates mainpaths,
    struct ptrnstates subpaths,
    struct ptrnstate  init)
{
    mainpaths = ptrn_state_put(mainpaths, init);

    while (mainpaths.end - mainpaths.bgn != 0) {
        // Step all the states, and return if matched.
        for (const struct ptrnstate* i = mainpaths.bgn; i < mainpaths.end;
             i++) {
            struct ptrndecoderes res = ptrn_decode(ctx, subpaths, *i);
            subpaths                 = res.states;
            if (res.matched) {
                return str_size(init.input) - str_size(i->input);
            }
        }
        // Take all the nondead subpaths to the main states.
        mainpaths = ptrn_state_clear(mainpaths);
        for (const struct ptrnstate* i = subpaths.bgn; i < subpaths.end; i++) {
            if (!i->dead) {
                mainpaths = ptrn_state_put(mainpaths, *i);
            }
        }
        subpaths = ptrn_state_clear(subpaths);
    }

    return NOT_MATCHED;
}

struct str ptrn_match(struct ptrnctx ctx, struct str name, struct str input)
{
    struct ptrnstates mainpaths = {0};
    struct ptrnstates subpaths  = {0};

    // TODO: Find pattern using the hash map.
    for (const struct ptrninfo* i = ctx.info.bgn; i < ctx.info.end; i++) {
        if (str_equal(i->name, name)) {
            // Create the initial state.
            struct ptrnstate init = {
                .input = input,
                .code  = i->code,
                .dead  = false};

            // If matches the pattern, return the match.
            ptr consumed = match(ctx, mainpaths, subpaths, init);
            if (consumed != NOT_MATCHED) {
                mainpaths = ptrn_state_destroy(mainpaths);
                subpaths  = ptrn_state_destroy(subpaths);
                ASSERT(
                    consumed <= str_size(input),
                    "Consumed more than the input!");
                return (
                    struct str){.bgn = input.bgn, .end = input.bgn + consumed};
            }

            // Clear the states to be reused later.
            mainpaths = ptrn_state_clear(mainpaths);
            subpaths  = ptrn_state_clear(subpaths);
        }
    }

    // Return empty string if nothing matched.
    mainpaths = ptrn_state_destroy(mainpaths);
    subpaths  = ptrn_state_destroy(subpaths);
    return (struct str){0};
}
