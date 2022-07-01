// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"
#include "buffer.hh"
#include "internal.hh"
#include "string/api.hh"

namespace cthrice
{
    static Pattern create(int64_t literal, size_t target_offset)
    {
        return {
            .type = Pattern::EDGE,
            .data = {
                .edge = {.literal = literal, .target_offset = target_offset}}};
    }

    static Pattern create(size_t edges)
    {
        return {.type = Pattern::VERTEX, .data = {.vertex = {.edges = edges}}};
    }

    static Pattern create(String name, bool visible)
    {
        return {
            .type = Pattern::MARKER,
            .data = {.marker = {.name = name, .visible = visible}}};
    }

    Buffer<Pattern> parse(Buffer<Pattern> bfr, String name, String ptrn)
    {
        return bfr;
    }
} // namespace cthrice
