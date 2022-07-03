// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error/api.h"
#include "pattern/api.h"
#include "pattern/internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdlib.h>

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

static ptr size(struct ptrns ptrns)
{
    ASSERT(ptrns.end >= ptrns.bgn, "Patterns do not have a positive size!");
    return ptrns.end - ptrns.bgn;
}

static ptr capacity(struct ptrns ptrns)
{
    ASSERT(ptrns.lst >= ptrns.bgn, "Patterns do not have a positive capacity!");
    return ptrns.lst - ptrns.bgn;
}

static ptr space(struct ptrns ptrns)
{
    ASSERT(ptrns.lst >= ptrns.end, "Patterns do not have a positive space!");
    return ptrns.lst - ptrns.end;
}

static struct ptrns grow(struct ptrns ptrns)
{
    ptr cap  = capacity(ptrns);
    ptr sze  = size(ptrns);
    ptr grow = max(8, cap / 2);
    ptr nwc  = grow + cap;

    ASSERT(nwc > 0, "New patterns capacity is not positive!");
    struct ptrn* mem = realloc(ptrns.bgn, nwc * sizeof(struct ptrn));
    CHECK(mem != 0, "Could not allocate patterns!");

    ptrns.bgn = mem;
    ptrns.end = mem + sze;
    ptrns.lst = mem + nwc;
    return ptrns;
}

static struct ptrns put(struct ptrns ptrns, struct ptrn ptrn)
{
    if (space(ptrns) < 1) {
        ptrns = grow(ptrns);
    }
    *ptrns.end++ = ptrn;
    return ptrns;
}

struct ctx {
    struct ptrns ptrns;
    struct str   ptrn;
};

static byte escape(byte b)
{
    switch (b) {
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
            return 0; // For fixing the warning.
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
        ctx.ptrns = put(ctx.ptrns, create(1));
        ctx.ptrns = put(ctx.ptrns, create(size(ctx.ptrns) + 1, c, 0));
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

static bool whitespace(byte b)
{
    return b == '\n' || b == '\r' || b == '\t' || b == ' ';
}

static struct ctx parse_marker(struct ctx ctx)
{
    // Trim whitespace.
    while (str_size(ctx.ptrn) > 0 && whitespace(*ctx.ptrn.bgn)) {
        ctx.ptrn.bgn++;
    }
    CHECK(str_size(ctx.ptrn) != 0, "Pattern does not have a name!");

    return ctx;
}

struct ptrns parse(struct ptrns ptrns, struct str ptrn)
{
    struct ctx ctx = {.ptrns = ptrns, .ptrn = ptrn};

    ctx = parse_marker(ctx);

    // Vertex that marks the start of a pattern.
    ptrns = put(ptrns, create(name, contains(name, '@')));

    while (str_size(ctx.ptrn) != 0) {
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
    ctx.ptrns = put(ctx.ptrns, create(0));
    return ctx.ptrns;
}
