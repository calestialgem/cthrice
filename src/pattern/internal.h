// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INTERNAL_H
#define INTERNAL_H 1

#include "string/api.h"
#include "types/api.h"

/** Implementation of the pattern. This is a graph where edges and verticies
 * are stored in an array. */
struct ptrn {
    /** Type of the pattern. */
    enum {
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
    } type;

    /** Data of the pattern. */
    union {
        struct {
            ptr  target_offset;
            byte literal;
            byte other;
        };
        struct {
            ptr edges;
        };
        struct {
            struct str name;
            bool       visible;
        };
    };
};

#endif // INTERNAL_H
