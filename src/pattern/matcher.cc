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

        int64_t literal = ptrn->data.edge.literal;

        if (literal != Pattern::Data::FREE) {
            // Check the literal.
            traversal.dead = literal != *traversal.str.bgn;

            // Consume the literal.
            traversal.str.bgn++;
        }

        return traversal;
    }

    bool match(Range<Pattern> rnge, String str, const Pattern* ptrn)
    {
        // DEBUG: Print all the patterns.
        print(rnge);

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
                const Pattern* ptrn = i->rnge.bgn + i->off;
                switch (ptrn->type) {
                    case Pattern::VERTEX:
                        if (ptrn->data.vertex.edges == 0) {
                            // A vertex with no edges represents a match.
                            bfr = destory(bfr);
                            return true;
                        }
                        // The vertex is not important anymore.
                        i->dead = true;
                        cthrice_check(
                            ptrn + ptrn->data.vertex.edges == rnge.end,
                            "Some of the edges that are claimed by the vertex "
                            "do not exist!");
                        for (size_t j = 0; j < ptrn->data.vertex.edges; j++) {
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
                        print(ptrn);
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
