// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>

/* Check whether the pattern is a free edge. Does not check whether the
 * pattern is an edge! */
static bool edge_free(struct ptrn ptrn)
{
    // If the literal character of the edge is zero, this is an
    // unconditional edge.
    return ptrn.literal == 0;
}

/* Check whether the pattern is a range of characters. Does not check
 * whether the pattern is an edge! */
static bool edge_range(struct ptrn ptrn)
{
    // If the other character of the edge is nonzero, this is an
    // range of characters.
    return ptrn.other != 0;
}

/* Check whether the character fits the literal or range pattern. Does not
 * check whether the pattern is an edge or free edge! */
static bool edge_check(struct ptrn ptrn, byte b)
{
    if (edge_range(ptrn)) {
        return b >= ptrn.literal && b <= ptrn.other;
    }
    return b == ptrn.literal;
}

static void print_edge(struct ptrn ptrn)
{
    printf("EDGE; ");
    if (edge_free(ptrn)) {
        printf("FREE");
    } else if (edge_range(ptrn)) {
        printf("{%c-%c}", ptrn.literal, ptrn.other);
    } else {
        printf("{%c}", ptrn.literal);
    }
    printf("; %05llu", ptrn.target_offset);
}

static void print_marker(struct ptrn ptrn)
{
    printf("MARKER; {%.*s}; ", (int)str_size(ptrn.name), ptrn.name.bgn);
    if (ptrn.visible) {
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
            printf("VERTEX; %llu", ptrn.edges);
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

struct trvl {
    struct str str;
    ptr        off;
    bool       dead;
};

static ptr ptrns_size(struct ptrns ptrns)
{
    ASSERT(ptrns.end >= ptrns.bgn, "Patterns has negative size!");
    return ptrns.end - ptrns.bgn;
}

struct trvl traverse_edge(struct ptrns ptrns, struct trvl trvl)
{
    struct ptrn ptrn = *(ptrns.bgn + trvl.off);
    ASSERT(ptrn.type == EDGE, "Cannot traverse something that is not an edge!");

    ASSERT(
        ptrn.target_offset >= ptrns_size(ptrns),
        "Target of the edge is out of bounds!");
    trvl.off = ptrn.target_offset;

    if (!edge_free(ptrn)) {
        // Check and consume the current character.
        trvl.dead = !edge_check(ptrn, *trvl.str.bgn++);
    }

    return trvl;
}

struct trvls {
    struct trvl* bgn;
    struct trvl* end;
    struct trvl* lst;
};

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
    ASSERT(trvl.off < ptrns_size(ptrns), "Current vertex is out of bounds!");
    struct ptrn ptrn = *(ptrns.bgn + trvl.off);

    switch (ptrn.type) {
        case VERTEX:
            if (ptrn.edges == 0) {
                // A vertex with no edges represents a match.
                return (struct vrtx_trvl){.trvls = trvls, .matched = true};
            }
            ASSERT(
                trvl.off + ptrn.edges < ptrns_size(ptrns),
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
                                .off  = trvl.off + 1 + j,
                                .dead = false}));
            }
            break;
        default:
            printf("Unexpected: [%05llu] ", trvl.off);
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
    // DEBUG: Print all the patterns.
    print_patterns(ptrns);

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
