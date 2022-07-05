// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer/api.h"
#include "error/api.h"
#include "pattern/api.h"
#include "pattern/internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>
#include <stdlib.h>

static struct ptrn create_edge(ptr target_offset, byte literal, byte other)
{
    return (struct ptrn){
        .type          = EDGE,
        .target_offset = target_offset,
        .literal       = literal,
        .other         = other};
}

static struct ptrn create_vertex(ptr edges)
{
    return (struct ptrn){.type = VERTEX, .edges = edges};
}

static struct ptrn create_marker(struct str name, bool visible)
{
    return (struct ptrn){.type = MARKER, .name = name, .visible = visible};
}

static ptr size(struct ptrnctx ptrns)
{
    ASSERT(ptrns.end >= ptrns.bgn, "Patterns do not have a positive size!");
    return ptrns.end - ptrns.bgn;
}

static ptr capacity(struct ptrnctx ptrns)
{
    ASSERT(
        ptrns.lst >= ptrnctx.bgn,
        "Patterns do not have a positive capacity!");
    return ptrns.lst - ptrns.bgn;
}

static ptr space(struct ptrns ptrns)
{
    ASSERT(ptrns.lst >= ptrns.end, "Patterns do not have a positive space!");
    return ptrns.lst - ptrns.end;
}

static struct ptrnctx grow(struct ptrnctx ptrns)
{
    ptr cap  = capacity(ptrns);
    ptr sze  = size(ptrns);
    ptr grow = max(8, cap / 2);
    ptr nwc  = grow + cap;

    ASSERT(nwc > 0, "New patterns capacity is not positive!");
    struct ptrn* mem = realloc(ptrns.bgn, nwc * sizeof(struct ptrn));
    CHECK(mem != null, "Could not allocate patterns!");

    ptrns.bgn = mem;
    ptrns.end = mem + sze;
    ptrns.lst = mem + nwc;
    return ptrns;
}

static struct ptrnctx put(struct ptrnctx ptrns, struct ptrn ptrn)
{
    if (space(ptrns) < 1) {
        ptrns = grow(ptrns);
    }
    *ptrns.end++ = ptrn;
    return ptrns;
}

/* Copy result together with the changed patterns. */
struct copy_res {
    struct ptrnctx ptrns;
    struct str     res;
};

/* Copy the string to the buffer of ptrns, such that all the pattern memory is
 * independent of the inputs. */
static struct copy_res copy_str(struct ptrnctx ptrns, struct str str)
{
    // Store the previous end of the buffer.
    ptr off   = bfr_size(ptrns.bfr);
    ptrns.bfr = bfr_put_str(ptrns.bfr, str);
    return (
        struct copy_res){.ptrns = ptrns, .res = bfr_view_sub(ptrns.bfr, off)};
}

struct ctx {
    struct ptrnctx ptrns;
    struct str     ptrn;
    bool           alter;
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
        case '~': // Tilde.
            return '~';
        default:
            CHECK(false, "Unknown escape sequence in character literal!");
            return 0; // For fixing the warning.
    }
}

static bool whitespace(byte b)
{
    return b == '\n' || b == '\r' || b == '\t' || b == ' ';
}

static bool not_whitespace(byte b)
{
    return !whitespace(b);
}

static struct ctx parse_marker(struct ctx ctx)
{
    // Trim whitespace.
    ctx.ptrn.bgn = str_find_pred(ctx.ptrn, &not_whitespace);

    // The first word is the name.
    struct str name = {
        .bgn = ctx.ptrn.bgn,
        .end = str_find_pred(ctx.ptrn, &whitespace)};
    CHECK(str_finite(name), "Pattern does not have a name!");

    // Check for visibility, and consume the token if it is there.
    const byte* token   = str_find(name, '@');
    bool        visible = token != name.end;

    if (visible) {
        CHECK(
            token == name.bgn,
            "Visibility token in the pattern name must come before it!");
        name.bgn++;
        CHECK(
            str_finite(name),
            "Pattern does not have a name after the visibility token!");
    }

    // Copy the name into the patterns buffer.
    struct copy_res copy_res = copy_str(ctx.ptrns, name);
    ctx.ptrns                = copy_res.ptrns;
    name                     = copy_res.res;

    // Create the vertex that marks the start of a pattern.
    ctx.ptrns = put(ctx.ptrns, create_marker(name, visible));

    // Consume the name of the pattern and visiblity token.
    ctx.ptrn.bgn += str_size(name) + visible;

    return ctx;
}

static struct ctx parse_literal(struct ctx ctx)
{
    CHECK(
        str_finite(ctx.ptrn),
        "No character literal in pattern after opening "
        "quotes!");

    ptr  len   = 0;
    bool range = false;

    while (str_finite(ctx.ptrn)) {
        byte b = *ctx.ptrn.bgn++;

        switch (b) {
            case '\'': // End of the quote.
                return ctx;
            case '~': // Character range.
                CHECK(
                    range != true,
                    "Misplaced character range token in pattern!");
                CHECK(
                    len == 1,
                    "Incorrectly structured character range in pattern!");
                range = true;
                CHECK(
                    str_finite(ctx.ptrn),
                    "No end of the character range after character range token "
                    "in pattern!");
                // Consume the range token.
                b = *ctx.ptrn.bgn++;
                break;
            case '\\': // Escape using backslash.
                CHECK(
                    str_finite(ctx.ptrn),
                    "No escaped character literal after backslash in pattern!");
                // Consume the escape character.
                b = escape(*ctx.ptrn.bgn++);
                break;
            default:
                break;
        }

        if (!range) {
            if (!ctx.alter) {
                // Create an edge that marks a character literal.
                ctx.ptrns = put(ctx.ptrns, create_vertex(1));
            } else {
                ctx.alter = false;
                // This is the start of an alternative path; thus, rather than
                // creating a new vertex, add the edge to the previous vertex.
                struct ptrn* vrtx = null;
                for (vrtx = ctx.ptrns.end - 1; vrtx >= ctx.ptrns.bgn; vrtx--) {
                    switch (vrtx->type) {
                        case EDGE:
                            vrtx->target_offset++;
                            break;
                        case VERTEX:
                            vrtx->edges++;
                            goto after;
                        case MARKER:
                            CHECK(
                                vrtx != null,
                                "Alternative operator at the start of the "
                                "pattern!");
                        default:
                            ASSERT(false, "Unknown node type!");
                    }
                }
            }
        after:
            ctx.ptrns = put(ctx.ptrns, create_edge(size(ctx.ptrns) + 1, b, 0));
            len++;
        } else {
            len++;
            CHECK(
                len == 2,
                "Incorrectly structured character range in pattern!");
            // Turn the previously created edge to a character range.
            ASSERT(size(ctx.ptrns) > 0, "There is no previous pattern!");
            struct ptrn* pre = ctx.ptrns.end - 1;

            ASSERT(pre->type == EDGE, "Previous pattern is not an edge!");
            pre->other = b;
        }
    }

    CHECK(
        false,
        "No closing quotes in pattern after character "
        "literal!");
    return ctx;
}

struct ptrnctx ptrn_parse(struct ptrnctx ptrns, struct str ptrn)
{
    struct ctx ctx = {.ptrns = ptrns, .ptrn = ptrn};
    ctx            = parse_marker(ctx);

    // Trim whitespace.
    ctx.ptrn.bgn = str_find_pred(ctx.ptrn, &not_whitespace);
    CHECK(str_finite(ctx.ptrn), "There is no pattern after the name!");

    while (str_finite(ctx.ptrn)) {
        switch (*ctx.ptrn.bgn++) {
            case '\r':
            case '\n':
            case '\t':
            case ' ':
                // Skip whitespace.
                break;
            case '\'':
                ctx = parse_literal(ctx);
                break;
            case '|':
                CHECK(
                    ctx.alter == false,
                    "Repeating alternative operator in pattern!");
                ctx.alter = true;
                break;
            default:
                CHECK(false, "Unexpected character in pattern!");
        }
    }

    // Vertex that marks the match.
    ctx.ptrns = put(ctx.ptrns, create_vertex(0));
    return ctx.ptrns;
}

struct ptrnctx ptrn_destory(struct ptrnctx ptrns)
{
    ptrns.bfr = bfr_destroy(ptrns.bfr);
    free(ptrns.bgn);
    ptrns.bgn = ptrns.end = ptrns.lst = null;
    return ptrns;
}
