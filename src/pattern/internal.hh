// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdint.h>
#include <vcruntime.h>
#ifndef INTERNAL_HH
#    define INTERNAL_HH 1

#    include "range.hh"
#    include "string/api.hh"

#    include <cstddef>
#    include <cstdint>

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

        static Pattern create(int64_t literal, size_t target_offset)
        {
            return {
                .type = EDGE,
                .data = {
                    .edge = {
                        .literal       = literal,
                        .target_offset = target_offset}}};
        }

        static Pattern create(size_t edges)
        {
            return {.type = VERTEX, .data = {.vertex = {.edges = edges}}};
        }

        static Pattern create(String name, bool visible)
        {
            return {
                .type = MARKER,
                .data = {.marker = {.name = name, .visible = visible}}};
        }
    };
} // namespace cthrice

#endif // INTERNAL_HH
