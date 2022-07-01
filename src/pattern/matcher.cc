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
        const Pattern* ptrn;
        bool           dead;
    };

    [[nodiscard]] Traversal traverse(Traversal traversal)
    {
        cthrice_check(
            traversal.ptrn->type != Pattern::EDGE,
            "Cannot traverse something that is not an edge!");

        const Pattern* target =
            traversal.rnge.bgn + traversal.ptrn->data.edge.target_offset;
        cthrice_check(
            target >= traversal.rnge.end,
            "Target of the edge is out of bounds!");

        int64_t literal = traversal.ptrn->data.edge.literal;
        traversal.ptrn  = target;

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
        // Skip the marker.
        cthrice_check(++ptrn == rnge.end, "Pattern ends after the marker!");

        Buffer<Traversal> bfr{};
        bfr = put(bfr, {.rnge = rnge, .str = str, .ptrn = ptrn, .dead = false});

        // Traverse all the avalible paths.
        while (size(bfr) != 0) {
            for (Traversal* i = bfr.bgn; i < bfr.end; i++) {
                if (i->dead) {
                    bfr = remove(bfr, i--);
                    continue;
                }
                switch (i->ptrn->type) {
                    case Pattern::VERTEX:
                        if (i->ptrn->data.vertex.edges == 0) {
                            // A vertex with no edges represents a match.
                            return true;
                        }
                        cthrice_check(
                            i->ptrn + i->ptrn->data.vertex.edges == rnge.end,
                            "Some of the edges that are claimed by the vertex "
                            "do not exist!");
                        for (size_t j = 0; j < i->ptrn->data.vertex.edges;
                             j++) {
                            // Traverse all the edges in this vertex.
                            bfr =
                                put(bfr,
                                    traverse(
                                        {.rnge = i->rnge,
                                         .str  = i->str,
                                         .ptrn = i->ptrn + 1 + j,
                                         .dead = false}));
                        }
                        // The vertex is not important anymore.
                        i->dead = true;
                        break;
                    default:
                        cthrice_check(
                            true,
                            "Unexpected node is found in the pattern!");
                }
            }
        }

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
