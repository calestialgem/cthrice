// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "internal.h"
#include "string/api.h"

struct ptrnctx ptrn_parse(struct ptrnctx ctx, struct str ptrn)
{
    struct ptrnlex lex = ptrn_lex(ptrn);

    // DEBUG: Print lex.
    ptrn_print_lex(lex);

    // TODO: Parse the lex.

    lex = ptrn_lex_destroy(lex);
    return ctx;
}

struct ptrnctx ptrn_destory(struct ptrnctx ctx)
{
    ctx     = ptrn_hash_destroy(ctx);
    ctx     = ptrn_code_destroy(ctx);
    ctx.bfr = bfr_destroy(ctx.bfr);
    return ctx;
}
