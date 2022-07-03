// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error/api.h"
#include "pattern/api.h"
#include "pattern/internal.h"
#include "types/api.h"

static struct ptrn create_edge(ptr target_offset, byte literal, byte other)
{
    return (struct ptrn){
        .type = EDGE,
        .data = {

                 .target_offset = target_offset,
                 .literal       = literal,
                 .other         = other}
    };
}

static struct ptrn create_vertex(ptr edges)
{
    return (struct ptrn){.type = VERTEX, .data = {.edges = edges}};
}

static struct ptrn create_marker(struct str name, bool visible)
{
    return (struct ptrn){
        .type = MARKER,
        .data = {.name = name, .visible = visible}
    };
}

struct ctx {
    struct ptrns ptrns;
    struct str   ptrn;
};

static byte escape(byte c)
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
            CHECK(false, "Unknown escape sequence in character literal!");
    }
}

static struct ctx literal(struct ctx ctx)
{
    // Consume opening quotes.
    ctx.ptrn.bgn++;
    CHECK(
        size(ctx.ptrn) != 0,
        "No character literal in pattern after opening "
        "quotes!");

    while (*ctx.ptrn.bgn != '\'') {
        byte c = *ctx.ptrn.bgn;

        // Escape using backslash.
        if (c == '\\') {
            CHECK(
                size(ctx.ptrn) != 0,
                "No escaped character literal in pattern after backslash!");
            // Consume the escape character.
            c = escape(*(++ctx.ptrn.bgn));
        }

        // Vertex that marks a character literal.
        ctx.bfr = put(ctx.bfr, create(1));
        ctx.bfr = put(ctx.bfr, create(size(ctx.bfr) + 1, c, 0));
        ctx.ptrn.bgn++;

        CHECK(
            size(ctx.ptrn) != 0,
            "No closing quotes in pattern after character "
            "literal!");
    }

    // Consume closing quotes.
    ctx.ptrn.bgn++;
    return ctx;
}

struct ptrns parse(struct ptrns bfr, struct str ptrn)
{
    // Vertex that marks the start of a pattern.
    bfr = put(bfr, create(name, contains(name, '@')));

    struct ctx ctx = {.bfr = bfr, .ptrn = ptrn};

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
                CHECK(false, "Unexpected character in pattern!");
        }
    }

    // Vertex that marks the match.
    ctx.bfr = put(ctx.bfr, create(0));
    return ctx.bfr;
}
