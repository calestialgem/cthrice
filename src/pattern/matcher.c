// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>

/* Transition in state in the nondeterministic finite automaton. */
struct trvl {
    /* Remaining input. */
    struct str str;
    /* Index of the current pattern code. */
    ptr code;
    /* Whether transition of the pattern code fails. */
    bool dead;
};

/* Dynamic array of all current traversals. */
struct trvls {
    /* Pointer to the first allocated traversal. */
    struct trvl* bgn;
    /* Pointer to the traversal after the last valid one. */
    struct trvl* end;
    /* Pointer to the traversal after the last allocated one. */
    struct trvl* lst;
};

/* Returns the amount of code in the patterns. */
static ptr code_size(struct ptrns ptrns)
{
    ASSERT(
        ptrns.code.end >= ptrns.code.bgn,
        "There are negative amount of codes in the pattern!");
    return ptrns.code.end - ptrns.code.bgn;
}

/* Do the transition in state. */
static struct trvl transition(struct ptrns ptrns, struct trvl trvl)
{
    ASSERT(trvl.dead == false, "Traversing dead path!");
    ASSERT(trvl.code >= 0, "Index out of the pattern codes!");
    ASSERT(trvl.code < code_size(ptrns), "Index out of the pattern codes!");
    struct ptrncode code = *(ptrns.code.bgn + trvl.code);

    switch (code.type) {
        case EMPTY:
            break;
        case LITERAL:
            // Check the next input and consume it.
            trvl.dead = !str_finite(trvl.str) || *trvl.str.bgn++ != code.ltrl;
            break;
        case RANGE:
            // Check the next input and consume it.
            trvl.dead =
                !str_finite(trvl.str) ||
                !(*trvl.str.bgn++ >= code.bgn && *trvl.str.bgn <= code.end);
            break;
        case PATTERN:
            ASSERT(false, "Pattern referencing is not implemented!");
        case DIVERGE:
        case MATCH:
            // DEBUG: Print the unexpected code.
            print_code(code);
            ASSERT(false, "Unexcepted pattern code type!");
        default:
            // DEBUG: Print the unknown code.
            print_code(code);
            ASSERT(false, "Unknown pattern code type!");
    }

    // Move to the target code.
    trvl.code += code.move;
    ASSERT(trvl.code >= 0, "Moving to out of the pattern codes!");
    ASSERT(trvl.code < code_size(ptrns), "Moving to out of the pattern codes!");

    return trvl;
}

/* Put a new transition in to the buffer. */
static struct trvls put(struct trvls trvls, struct trvl trvl)
{
    CHECK(
        trvls.lst - trvls.end > 0,
        "No more space left to put the traversal!");
    *trvls.end++ = trvl;
    return trvls;
}

struct stepres {
    struct trvls trvls;
    bool         matched;
};

/* Do all avalible transitions in the current state. */
static struct stepres
step(struct ptrns ptrns, struct trvls trvls, struct trvl trvl)
{
    ASSERT(trvl.code >= 0, "Moving to out of the pattern codes!");
    ASSERT(trvl.code < code_size(ptrns), "Moving to out of the pattern codes!");
    struct ptrncode code = *(ptrns.code.bgn + trvl.code);

    switch (code.type) {
        case DIVERGE:
            ASSERT(
                code.amt > 0,
                "The pattern does not diverge to positive amount of paths!");
            ASSERT(
                trvl.code + code.amt < code_size(ptrns),
                "Some of the paths that are do not exist in the pattern!");
            for (ptr j = 0; j < code.amt; j++) {
                trvl.code++;
                trvls = put(trvls, transition(ptrns, trvl));
            }
            break;
        case MATCH:
            return (struct stepres){.trvls = trvls, .matched = true};
        default:
            trvls = put(trvls, transition(ptrns, trvl));
    }

    return (struct stepres){.trvls = trvls, .matched = false};
}

/* Run the code for the given pattern. */
static bool code_run(struct ptrns ptrns, struct trvls trvls)
{
    while (trvls.end - trvls.bgn != 0) {
        for (ptr i = 0; i < trvls.end - trvls.bgn; i++) {
            // Remove the dead traversals.
            struct trvl trvl = trvls.bgn[i];
            if (trvl.dead) {
                trvls.bgn[i--] = *--trvls.end;
                continue;
            }
            // Traverse the vertex, and return if matched.
            struct stepres res = step(ptrns, trvls, trvl);
            if (res.matched) {
                return true;
            }
            trvls = res.trvls;

            // Remove the traversed vertex.
            trvls.bgn[i--] = *--trvls.end;
        }
    }

    return false;
}

static bool match(struct ptrns ptrns, struct trvl trvl)
{
    // Use stack memory for traversals.
#define TRAVERSAL_CAPACITY 32
    struct trvl  memory[TRAVERSAL_CAPACITY];
    struct trvls trvls = {
        .bgn = memory,
        .end = memory,
        .lst = memory + TRAVERSAL_CAPACITY};

    // Put the initial state.
    trvls = put(trvls, trvl);
    return code_run(ptrns, trvls);
}

struct str ptrn_match(struct ptrns ptrns, struct str str)
{
    for (const struct ptrn* i = ptrns.values.bgn; i < ptrns.values.end; i++) {
        if (i->visible) {
            struct trvl init = {.str = str, .code = i->code, .dead = false};
            if (match(ptrns, init)) {
                // If matches a visible pattern, return the name.
                return i->name;
            }
        }
    }

    // Return empty string if nothing matched.
    return (struct str){0};
}

bool ptrn_check(struct ptrns ptrns, struct str name, struct str str)
{
    // TODO: Use hashmap to access to the pattern code.
    return false;
}
