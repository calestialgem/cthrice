// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>

/* Check whether the pattern is a free edge. Does not check whether the
 * pattern is an edge! */
static bool free(struct ptrn ptrn)
{
    // If the literal character of the edge is zero, this is an
    // unconditional edge.
    return ptrn.data.literal == 0;
}

/* Check whether the pattern is a range of characters. Does not check
 * whether the pattern is an edge! */
static bool range(struct ptrn ptrn)
{
    // If the other character of the edge is nonzero, this is an
    // range of characters.
    return ptrn.data.other != 0;
}

/* Check whether the character fits the literal or range pattern. Does not
 * check whether the pattern is an edge or free edge! */
static bool check(struct ptrn ptrn, byte b)
{
    if (range(ptrn)) {
        return b >= ptrn.data.literal && b <= ptrn.data.other;
    }
    return b == ptrn.data.literal;
}

static void print_edge(struct ptrn ptrn)
{
    printf("EDGE; ");
    if (free(ptrn)) {
        printf("FREE");
    } else if (range(ptrn)) {
        printf("{%c-%c}", ptrn.data.literal, ptrn.data.other);
    } else {
        printf("{%c}", ptrn.data.literal);
    }
    printf("; %05llu", ptrn.data.target_offset);
}

static void print_marker(struct ptrn ptrn)
{
    printf(
        "MARKER; {%.*s}; ",
        (int)str_size(ptrn.data.name),
        ptrn.data.name.bgn);
    if (ptrn.data.visible) {
        printf("Visible");
    } else {
        printf("Invisible");
    }
}

/* Print the pattern. */
static void print_pattern(struct ptrn ptrn)
{
    printf("{ ");
    switch (ptrn.type) {
        case EDGE:
            print_edge(ptrn);
            break;
        case VERTEX:
            printf("VERTEX; %llu", ptrn.data.edges);
            break;
        case MARKER:
            print_marker(ptrn);
            break;
        default:
            printf("UNKNOWN");
    }
    printf(" }\n");
}

/* Print all the patterns in the range with their offset. */
static void print_patterns(struct ptrns ptrns)
{
    printf("\nPatterns:\n---------\n");
    for (const struct ptrn* i = ptrns.bgn; i < ptrns.end; i++) {
        printf("[%05llu] ", i - ptrns.bgn);
        print_pattern(*i);
    }
}

struct Traversal {
    struct ptrns ptrns;
    struct str   str;
    size_t       off;
    bool         dead;
};

struct Traversal traverse(struct Traversal traversal)
{
    struct ptrn ptrn = traversal.ptrns.bgn + traversal.off;

    cthrice_check(
        ptrn->type != EDGE,
        "Cannot traverse something that is not an edge!");

    cthrice_check(
        ptrn->data.target_offset >= size(traversal.ptrns),
        "Target of the edge is out of bounds!");
    traversal.off = ptrn->data.target_offset;

    if (!free(ptrn)) {
        // Check and consume the current character.
        traversal.dead = !check(ptrn, *traversal.str.bgn++);
    }

    return traversal;
}

bool match(struct ptrns ptrns, struct str str, struct ptrn ptrn)
{
    // // DEBUG: Print all the patterns.
    // print(ptrns);

    // Skip the marker.
    cthrice_check(++ptrn == ptrns.end, "struct ptrn ends after the marker!");

    Buffer<struct Traversal> bfr{};
    bfr =
        put(bfr,
            {.ptrns = ptrns,
             .str   = str,
             .off   = (size_t)(ptrn - ptrns.bgn),
             .dead  = false});

    // Traverse all the avalible paths.
    while (size(bfr) != 0) {
        for (struct Traversal* i = bfr.bgn; i < bfr.end; i++) {
            if (i->dead) {
                bfr = remove(bfr, i--);
                continue;
            }
            cthrice_check(
                i->off >= size(i->ptrns),
                "Current vertex is out of bounds!");
            struct ptrn p = i->ptrns.bgn + i->off;
            switch (p->type) {
                case VERTEX:
                    if (p.data.edges == 0) {
                        // A vertex with no edges represents a match.
                        bfr = destory(bfr);
                        return true;
                    }
                    // The vertex is not important anymore.
                    i->dead = true;
                    cthrice_check(
                        p + p.data.edges == ptrns.end,
                        "Some of the edges that are claimed by the vertex "
                        "do not exist!");
                    for (size_t j = 0; j < p.data.edges; j++) {
                        // Traverse all the edges in this vertex.
                        bfr =
                            put(bfr,
                                traverse(
                                    {.ptrns = i->ptrns,
                                     .str   = i->str,
                                     .off   = i->off + 1 + j,
                                     .dead  = false}));
                    }
                    break;
                default:
                    printf("Unexpected: [%05llu] ", i->off);
                    print_patterns(p);
                    cthrice_check(
                        true,
                        "Unexpected node is found in the pattern!");
            }
        }
    }

    bfr = destory(bfr);
    return false;
}

struct str match(struct ptrns ptrns, struct str str)
{
    for (struct ptrn i = ptrns.bgn; i < ptrns.end; i++) {
        if (i->type == MARKER && i->data.visible && match(ptrns, str, i)) {
            // If matches a visible pattern, return the name.
            return i->data.name;
        }
    }

    // Return empty string if nothing matched.
    return {};
}
