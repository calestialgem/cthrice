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

    struct ParseContext {
        Buffer<Pattern> bfr;
        String          ptrn;
    };

    [[nodiscard]] static ParseContext literal(ParseContext ctx)
    {
        ctx.ptrn.bgn++;
        cthrice_check(
            size(ctx.ptrn) == 0,
            "No character literal in pattern after opening "
            "quotes!");

        // Vertex that marks a character literal.
        ctx.bfr = put(ctx.bfr, create(1));
        ctx.bfr = put(ctx.bfr, create(*ctx.ptrn.bgn, size(ctx.bfr) + 1));
        ctx.ptrn.bgn++;

        cthrice_check(
            size(ctx.ptrn) < 2,
            "No closing quotes in pattern after character "
            "literal!");
        ctx.ptrn.bgn++;

        return ctx;
    }

    Buffer<Pattern> parse(Buffer<Pattern> bfr, String name, String ptrn)
    {
        // Vertex that marks the start of a pattern.
        bfr = put(bfr, create(name, contains(name, '@')));

        ParseContext ctx = {.bfr = bfr, .ptrn = ptrn};

        while (size(ptrn) != 0) {
            switch (*ptrn.bgn) {
                case '\'':
                    ctx = literal(ctx);
                    break;
                default:
                    cthrice_check(true, "Unexpected character in pattern!");
            }
        }

        // Vertex that marks the match.
        bfr = put(bfr, create(0));
        return bfr;
    }
} // namespace cthrice
