// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdlib.h>

/* Current state of the lexing process. */
struct ctx {
    /* List of tokens. */
    struct ptrnlex lex;
    /* Remaining input string. */
    struct str input;
};

/* Result of a sub lex operation. */
struct res {
    /* Resulting context. */
    struct ctx ctx;
    /* Whether a lex was done. */
    bool lxd;
};

/* Add a token to the lex and return a positive result. */
static struct res add(struct ctx ctx, enum ptrntkntyp typ, ptr len)
{
    struct ptrntkn tkn = {
        .typ = typ,
        .val = {.bgn = ctx.input.bgn, .end = ctx.input.bgn + len}
    };
    ctx.input.bgn += len;

    // Grow if the current avalible space is not enough.
    if (ctx.lex.lst - ctx.lex.end < 1) {
        // Store the current size to calculate the end pointer later.
        ptr sze = ctx.lex.end - ctx.lex.bgn;
        ASSERT(sze >= 0, "Size is negative!");

        // Double the capacity.
        ptr cap = ctx.lex.lst - ctx.lex.bgn;
        ASSERT(cap >= 0, "Capacity is negative");
        ptr nwc = max(1, cap << 1);

        struct ptrntkn* mem = realloc(ctx.lex.bgn, nwc);
        CHECK(mem != null, "Could not allocate token!");

        ctx.lex.bgn = mem;
        ctx.lex.end = mem + sze;
        ctx.lex.lst = mem + nwc;
    }

    *ctx.lex.end++ = tkn;
    return (struct res){.ctx = ctx, .lxd = true};
}

/* Try to lex a punctuation mark. */
static struct res mark(struct ctx ctx)
{
    ASSERT(str_finite(ctx.input), "There is no input!");
    const struct str marks = str_convert("=.|,?*+{}[]");
    const byte*      i     = str_find(marks, *ctx.input.bgn);
    if (i == marks.end) {
        return (struct res){.ctx = ctx, .lxd = false};
    }

    return add(ctx, TOKEN_EQUAL + i - marks.bgn, 1);
}

/* Check whether a character is not a decimal digit. */
static bool not_digit(byte c)
{
    return c < '0' || c > '9';
}

/* Try to lex a number. */
static struct res number(struct ctx ctx)
{
    ASSERT(str_finite(ctx.input), "There is no input!");
    const byte* i = str_find_pred(ctx.input, &not_digit);
    if (i == ctx.input.bgn) {
        return (struct res){.ctx = ctx, .lxd = false};
    }

    return add(ctx, TOKEN_NUMBER, i - ctx.input.bgn);
}

/* Try to lex a quote. */
static struct res quote(struct ctx ctx)
{
    ASSERT(str_finite(ctx.input), "There is no input!");
    if (*ctx.input.bgn != '\'') {
        return (struct res){.ctx = ctx, .lxd = false};
    }
    const byte* crt = ctx.input.bgn;

    while (++crt < ctx.input.end) {
        switch (*crt) {
            case '\'':
                break;
            case '\\':
                crt++;
            default:
                continue;
        }
        break;
    }

    ptr len = crt - ctx.input.bgn;
    CHECK(len != 1 && *(crt - 1) == '\'', "No closing quote!");
    return add(ctx, TOKEN_QUOTE, len);
}

/* Check whether a character is not a letter or underscore. */
static bool not_letter_underscore(byte c)
{
    return (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_';
}

/* Try to lex an identifier. */
static struct res identifier(struct ctx ctx)
{
    ASSERT(str_finite(ctx.input), "There is no input!");
    const byte* i = str_find_pred(ctx.input, &not_letter_underscore);
    if (i == ctx.input.bgn) {
        return (struct res){.ctx = ctx, .lxd = false};
    }

    return add(ctx, TOKEN_IDENTIFIER, i - ctx.input.bgn);
}

/* Check whether a character is not a whitespace. */
static bool not_whitespace(byte c)
{
    return c != '\t' && c != '\n' && c != '\r' && c != ' ';
}

/* Skip all the whitespace. */
static struct str trim(struct str str)
{
    str.bgn = str_find_pred(str, &not_whitespace);
    return str;
}

/* Lex the next token from the input. */
static struct ctx next(struct ctx ctx)
{
    ctx.input      = trim(ctx.input);
    struct res res = {0};

    res = mark(ctx);
    if (res.lxd) {
        return res.ctx;
    }

    res = number(ctx);
    if (res.lxd) {
        return res.ctx;
    }

    res = quote(ctx);
    if (res.lxd) {
        return res.ctx;
    }

    res = identifier(ctx);
    if (res.lxd) {
        return res.ctx;
    }

    // Use -1 for unknown token type.
    return add(ctx, -1, 1).ctx;
}

struct ptrnlex ptrn_lex(struct str input)
{
    struct ctx ctx = {.input = input};
    while (str_finite(ctx.input)) {
        ctx = next(ctx);
    }
    return ctx.lex;
}

struct ptrnlex ptrn_lex_destroy(struct ptrnlex lex)
{
    free(lex.bgn);
    lex.bgn = lex.end = lex.lst = null;
    return lex;
}
