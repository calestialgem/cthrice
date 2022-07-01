// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"
#include "error.hh"
#include "internal.hh"
#include "string/api.hh"

#include <vector>

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

    size_t undead(std::vector<Traversal> traversals)
    {
        size_t res = 0;
        for (const Traversal& t : traversals) {
            if (!t.dead) {
                res++;
            }
        }
        return res;
    }

    bool match(Range<Pattern> rnge, String str, const Pattern* ptrn)
    {
        // Skip the marker.
        cthrice_check(++ptrn == rnge.end, "Pattern ends after the marker!");

        // Traverse all the avalible paths.
        std::vector<Traversal> traversals{};
        traversals.push_back(
            {.rnge = rnge, .str = str, .ptrn = ptrn, .dead = false});

        while (undead(traversals) != 0) {
            for (auto i = traversals.begin(); i < traversals.end(); i++) {
                if (i->dead) {
                    // traversals.erase(i);
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
                            traversals.push_back(traverse(
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
