// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"
#include "buffer.hh"
#include "error.hh"
#include "internal.hh"
#include "range.hh"
#include "string/api.hh"

#include <cstdio>

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

    static char escape(char c)
    {
        switch (c) {
            case 'a': // Alert.
                return '\a';
            case 'b': // Backspace.
                return '\b';
            case 'f': // Formfeed Page Break.
                return '\f';
            case 'n': // Newline.
                return '\n';
            case 'r': // Carriage Return.
                return '\r';
            case 't': // Horizontal Tab.
                return '\t';
            case 'v': // Vertical Tab.
                return '\v';
            case '\\': // Backslash.
                return '\\';
            case '\'': // Apostrophe.
                return '\'';
            case '"': // Double Quotation Mark.
                return '"';
            default:
                cthrice_check(
                    true,
                    "Unknown escape sequence in character literal!");
        }
    }

    [[nodiscard]] static ParseContext literal(ParseContext ctx)
    {
        // Consume opening quotes.
        ctx.ptrn.bgn++;
        cthrice_check(
            size(ctx.ptrn) == 0,
            "No character literal in pattern after opening "
            "quotes!");

        while (*ctx.ptrn.bgn != '\'') {
            char c = *ctx.ptrn.bgn;

            // Escape using backslash.
            if (c == '\\') {
                cthrice_check(
                    size(ctx.ptrn) == 0,
                    "No escaped character literal in pattern after backslash!");
                // Consume the escape character.
                c = escape(*(++ctx.ptrn.bgn));
            }

            // Vertex that marks a character literal.
            ctx.bfr = put(ctx.bfr, create(1));
            ctx.bfr = put(ctx.bfr, create(c, size(ctx.bfr) + 1));
            ctx.ptrn.bgn++;

            cthrice_check(
                size(ctx.ptrn) == 0,
                "No closing quotes in pattern after character "
                "literal!");
        }

        // Consume closing quotes.
        ctx.ptrn.bgn++;
        return ctx;
    }

    Buffer<Pattern> parse(Buffer<Pattern> bfr, String name, String ptrn)
    {
        // Vertex that marks the start of a pattern.
        bfr = put(bfr, create(name, contains(name, '@')));

        ParseContext ctx = {.bfr = bfr, .ptrn = ptrn};

        while (size(ctx.ptrn) != 0) {
            switch (*ctx.ptrn.bgn) {
                case '\r':
                case '\n':
                case '\t':
                case ' ':
                    ctx.ptrn.bgn++;
                    // Skip whitespace.
                    break;
                case '\'':
                    ctx = literal(ctx);
                    break;
                default:
                    cthrice_check(true, "Unexpected character in pattern!");
            }
        }

        // Vertex that marks the match.
        ctx.bfr = put(ctx.bfr, create(0));
        return ctx.bfr;
    }
} // namespace cthrice
