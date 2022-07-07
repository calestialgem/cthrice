// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "buffer/api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"

#include <stdio.h>

/* Current state of the parse. */
struct parse {
    /* Pattern context. */
    struct ptrnctx ctx;
    /* Name of the parsed pattern. */
    struct str name;
    /* Token array. */
    struct ptrnlex lex;
    /* The token that is currently on the carret. */
    struct ptrntkn* crt;
};

/* Find the remaining amount of tokens including the carret. */
static ix remaining(struct parse p)
{
    return p.lex.end - p.crt;
}

/* Parse the name of the pattern. */
static struct parse parse_name(struct parse p)
{
    CHECK(
        remaining(p) >= 3,
        "There should be atleast a name, an equal sign and a unit!");

    CHECK(
        p.crt->typ == TOKEN_IDENTIFIER,
        "Pattern definition must start with the identifier!");
    p.name = p.crt->val;
    p.crt++;

    CHECK(
        p.crt->typ == TOKEN_EQUAL,
        "Pattern definition must continue with an equal sign!");
    p.crt++;

    // TODO: Copy the name to the buffer.

    return p;
}

struct ptrnctx ptrn_parse(struct ptrnctx ctx, struct str ptrn)
{
    struct ptrnlex lex = ptrn_lex(ptrn);

    // DEBUG: Print lex.
    ptrn_print_lex(lex);

    struct parse p = {.ctx = ctx, .lex = lex, .crt = lex.bgn};
    p              = parse_name(p);

    p.lex = ptrn_lex_destroy(p.lex);
    return p.ctx;
}

struct ptrnctx ptrn_destory(struct ptrnctx ctx)
{
    ctx     = ptrn_hash_destroy(ctx);
    ctx     = ptrn_code_destroy(ctx);
    ctx.bfr = bfr_destroy(ctx.bfr);
    return ctx;
}
