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

/* Do the transition in state. */
struct trvl traverse(struct ptrns ptrns, struct trvl trvl)
{
    ASSERT(trvl.dead == false, "Traversing dead path!");
    ASSERT(trvl.code >= 0, "Index out of the pattern codes!");
    ASSERT(
        trvl.code < ptrns.code.end - ptrns.code.bgn,
        "Index out of the pattern codes!");
    struct ptrncode code = *(ptrns.code.bgn + trvl.code);

    switch (code.type) {
        case EMPTY:
            break;
        case LITERAL:
            // Check the next input and consume it.
            trvl.dead = *trvl.str.bgn++ != code.ltrl;
            break;
        case RANGE:
            // Consume the next input.
            byte b = *trvl.str.bgn++;
            // Check the input.
            trvl.dead = b >= code.bgn && b <= code.end;
            break;
        case PATTERN:
            ASSERT(false, "Pattern referencing is not implemented!");
        case DIVERGE:
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
    ASSERT(
        trvl.code < ptrns.code.end - ptrns.code.bgn,
        "Moving to out of the pattern codes!");

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

struct vrtx_trvl {
    struct trvls trvls;
    bool         matched;
};

static struct vrtx_trvl
traverse_vertex(struct ptrns ptrns, struct trvls trvls, struct trvl trvl)
{
    ASSERT(trvl.code >= 0, "Moving to out of the pattern codes!");
    ASSERT(
        trvl.code < ptrns.code.end - ptrns.code.bgn,
        "Moving to out of the pattern codes!");
    struct ptrncode code = *(ptrns.code.bgn + trvl.code);

    switch (ptrn.type) {
        case VERTEX:
            if (ptrn.edges == 0) {
                // A vertex with no edges represents a match.
                return (struct vrtx_trvl){.trvls = trvls, .matched = true};
            }
            ASSERT(
                trvl.code + ptrn.edges < ptrns_size(ptrns),
                "Some of the edges that are claimed by the vertex do not "
                "exist!");
            for (ptr j = 0; j < ptrn.edges; j++) {
                // Traverse all the edges in this vertex.
                trvls =
                    put(trvls,
                        traverse_edge(
                            ptrns,
                            (struct trvl){
                                .str  = trvl.str,
                                .code = trvl.code + 1 + j,
                                .dead = false}));
            }
            break;
        default:
            printf("Unexpected: [%05llu] ", trvl.code);
            print_patterns(ptrns);
            CHECK(false, "Unexpected node is found in the pattern!");
    }

    return (struct vrtx_trvl){.trvls = trvls, .matched = false};
}

/* Traverse all the avalible paths. */
static bool traverse_all_paths(struct ptrns ptrns, struct trvls trvls)
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
            struct vrtx_trvl res = traverse_vertex(ptrns, trvls, trvl);
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

static bool match(struct ptrns ptrns, struct str str, const struct ptrn* ptrn)
{
    // Skip the marker.
    ptrn++;
    ASSERT(ptrn < ptrns.end, "Pattern ends after the marker!");

    // Use stack memory for traversals.
#define TRAVERSAL_CAPACITY 32
    struct trvl  memory[TRAVERSAL_CAPACITY];
    struct trvls trvls = {
        .bgn = memory,
        .end = memory,
        .lst = memory + TRAVERSAL_CAPACITY};

    // Put initial traversal that comes after the marker.
    trvls =
        put(trvls,
            (struct trvl){
                .str  = str,
                .off  = ptrn - ptrns.bgn,
                .dead = false,
            });

    return traverse_all_paths(ptrns, trvls);
}

struct str ptrn_match(struct ptrns ptrns, struct str str)
{
    for (const struct ptrn* i = ptrns.bgn; i < ptrns.end; i++) {
        if (i->type == MARKER && i->visible && match(ptrns, str, i)) {
            // If matches a visible pattern, return the name.
            return i->name;
        }
    }

    // Return empty string if nothing matched.
    return (struct str){0};
}
