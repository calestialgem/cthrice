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
            /** Edge in the tree that checks and consumes a character that
             * equals a particular character or is in a character range. Free
             * edge means move to the vertex pointed by this edge without
             * checking and consuming a character. */
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
            struct {
                size_t target_offset;
                char   literal;
                char   other;
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
} // namespace cthrice

#endif // INTERNAL_HH
