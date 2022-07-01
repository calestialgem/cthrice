// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_HH
#define INTERNAL_HH 1

#include "range.hh"
#include "string/api.hh"

#include <cstddef>
#include <cstdint>

namespace cthrice
{
    /** Implementation of the pattern. This is a graph where edges and verticies
     * are stored in an array. */
    struct Pattern {
        /** Type of the pattern. */
        enum Type {
            /** Edge in the tree that checks a character. Free character
             * means move to the vertex pointed by this edge without checking
             * and consuming a character. */
            EDGE,
            /** Vertex in the tree that records how many edges come after this
             * vertex. Zero edges means the matching finished with success. */
            VERTEX,
            /** Marker is the start of a tree, stores the name and the
             * visibility. The pattern can be matched only if it is visible. */
            MARKER
        };

        /** Data of the pattern. */
        union Data {
            static constexpr int64_t FREE = -1;
            struct {
                int64_t literal;
                size_t  target_offset;
            } edge;
            struct {
                size_t edges;
            } vertex;
            struct {
                String name;
                bool   visible;
            } marker;
        };

        Type type;
        Data data;
    };

    inline void print(const Pattern* i)
    {
        printf("{ ");
        switch (i->type) {
            case Pattern::EDGE:
                printf("EDGE; ");
                if (i->data.edge.literal == Pattern::Data::FREE) {
                    printf("FREE");
                } else {
                    printf("{%c}", (char)i->data.edge.literal);
                }
                printf("; %05llu", i->data.edge.target_offset);
                break;
            case Pattern::VERTEX:
                printf("VERTEX; %llu", i->data.vertex.edges);
                break;
            case Pattern::MARKER:
                printf(
                    "MARKER; {%.*s}; ",
                    (int)size(i->data.marker.name),
                    i->data.marker.name.bgn);
                if (i->data.marker.visible) {
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

    inline void print(Range<Pattern> rnge)
    {
        printf("\nPatterns:\n---------\n");
        for (const Pattern* i = rnge.bgn; i < rnge.end; i++) {
            printf("[%05llu] ", i - rnge.bgn);
            print(i);
        }
    }
} // namespace cthrice

#endif // INTERNAL_HH
