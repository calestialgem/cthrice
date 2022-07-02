// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"
#include "buffer.hh"
#include "error.hh"
#include "internal.hh"
#include "range.hh"
#include "string/api.hh"

namespace cthrice
{
    /** Check whether the pattern is a free edge. Does not check whether the
     * pattern is an edge! */
    static bool free(const Pattern* p)
    {
        // If the literal character of the edge is zero, this is an
        // unconditional edge.
        return p->data.edge.literal == 0;
    }

    /** Check whether the pattern is a range of characters. Does not check
     * whether the pattern is an edge! */
    static bool range(const Pattern* p)
    {
        // If the other character of the edge is nonzero, this is an
        // range of characters.
        return p->data.edge.other != 0;
    }

    /** Check whether the character fits the literal or range pattern. Does not
     * check whether the pattern is an edge or free edge! */
    static bool check(const Pattern* p, char c)
    {
        if (range(p)) {
            return c >= p->data.edge.literal && c <= p->data.edge.other;
        }
        return c == p->data.edge.literal;
    }

    /** Print the pattern. */
    static void print(const Pattern* p)
    {
        printf("{ ");
        switch (p->type) {
            case Pattern::EDGE:
                printf("EDGE; ");
                if (free(p)) {
                    printf("FREE");
                } else if (range(p)) {
                    printf(
                        "{%c-%c}",
                        (char)p->data.edge.literal,
                        (char)p->data.edge.other);
                } else {
                    printf("{%c}", (char)p->data.edge.literal);
                }
                printf("; %05llu", p->data.edge.target_offset);
                break;
            case Pattern::VERTEX:
                printf("VERTEX; %llu", p->data.vertex.edges);
                break;
            case Pattern::MARKER:
                printf(
                    "MARKER; {%.*s}; ",
                    (int)size(p->data.marker.name),
                    p->data.marker.name.bgn);
                if (p->data.marker.visible) {
                    printf("Visible");
                } else {
                    printf("Invisible");
                }
                break;
            default:
                printf("UNKNOWN");
        }
        printf(" }\n");
    }

    /** Print all the patterns in the range with their offset. */
    static void print(Range<Pattern> rnge)
    {
        printf("\nPatterns:\n---------\n");
        for (const Pattern* i = rnge.bgn; i < rnge.end; i++) {
            printf("[%05llu] ", i - rnge.bgn);
            print(i);
        }
    }

    struct Traversal {
        Range<Pattern> rnge;
        String         str;
        size_t         off;
        bool           dead;
    };

    [[nodiscard]] Traversal traverse(Traversal traversal)
    {
        const Pattern* ptrn = traversal.rnge.bgn + traversal.off;

        cthrice_check(
            ptrn->type != Pattern::EDGE,
            "Cannot traverse something that is not an edge!");

        cthrice_check(
            ptrn->data.edge.target_offset >= size(traversal.rnge),
            "Target of the edge is out of bounds!");
        traversal.off = ptrn->data.edge.target_offset;

        if (!free(ptrn)) {
            // Check and consume the current character.
            traversal.dead = !check(ptrn, *traversal.str.bgn++);
        }

        return traversal;
    }

    bool match(Range<Pattern> rnge, String str, const Pattern* ptrn)
    {
        // // DEBUG: Print all the patterns.
        // print(rnge);

        // Skip the marker.
        cthrice_check(++ptrn == rnge.end, "Pattern ends after the marker!");

        Buffer<Traversal> bfr{};
        bfr =
            put(bfr,
                {.rnge = rnge,
                 .str  = str,
                 .off  = (size_t)(ptrn - rnge.bgn),
                 .dead = false});

        // Traverse all the avalible paths.
        while (size(bfr) != 0) {
            for (Traversal* i = bfr.bgn; i < bfr.end; i++) {
                if (i->dead) {
                    bfr = remove(bfr, i--);
                    continue;
                }
                cthrice_check(
                    i->off >= size(i->rnge),
                    "Current vertex is out of bounds!");
                const Pattern* p = i->rnge.bgn + i->off;
                switch (p->type) {
                    case Pattern::VERTEX:
                        if (p->data.vertex.edges == 0) {
                            // A vertex with no edges represents a match.
                            bfr = destory(bfr);
                            return true;
                        }
                        // The vertex is not important anymore.
                        i->dead = true;
                        cthrice_check(
                            p + p->data.vertex.edges == rnge.end,
                            "Some of the edges that are claimed by the vertex "
                            "do not exist!");
                        for (size_t j = 0; j < p->data.vertex.edges; j++) {
                            // Traverse all the edges in this vertex.
                            bfr =
                                put(bfr,
                                    traverse(
                                        {.rnge = i->rnge,
                                         .str  = i->str,
                                         .off  = i->off + 1 + j,
                                         .dead = false}));
                        }
                        break;
                    default:
                        printf("Unexpected: [%05llu] ", i->off);
                        print(p);
                        cthrice_check(
                            true,
                            "Unexpected node is found in the pattern!");
                }
            }
        }

        bfr = destory(bfr);
        return false;
    }

    String match(Range<Pattern> rnge, String str)
    {
        for (const Pattern* i = rnge.bgn; i < rnge.end; i++) {
            if (i->type == Pattern::MARKER && i->data.marker.visible &&
                match(rnge, str, i)) {
                // If matches a visible pattern, return the name.
                return i->data.marker.name;
            }
        }

        // Return empty string if nothing matched.
        return {};
    }
} // namespace cthrice
