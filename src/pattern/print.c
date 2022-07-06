// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "internal.h"

#include <stdio.h>

void print_infos(struct ptrnctx ctx)
{
    printf("\n  PATTERNS\n============\n\n");
    for (const struct ptrninfo* i = ctx.info.bgn; i < ctx.info.end; i++) {
        printf(
            "Pattern {%.*s} [%05lld]\n",
            (int)str_size(i->name),
            i->name.bgn,
            i->code);
    }
}

void print_codes(struct ptrnctx ctx)
{
    printf("\n  PATTERN CODE\n================\n\n");
    for (const struct ptrncode* i = ctx.code.bgn; i < ctx.code.end; i++) {
        printf("[%05lld] ", i - ctx.code.bgn);
        print_code(*i);
    }
}

void print_code(struct ptrncode code)
{
    switch (code.type) {
        case EMPTY:
            printf("EMPTY");
            break;
        case LITERAL:
            printf("LITERAL {%c}", code.ltrl);
            break;
        case RANGE:
            printf("RANGE {%c~%c}", code.bgn, code.end);
            break;
        case PATTERN:
            printf("PATTERN {%05lld}", code.indx);
            break;
        case DIVERGE:
            printf("DIVERGE {%lld}", code.amt);
            break;
        case MATCH:
            printf("MATCH");
            break;
    }
    printf(" %+lld\n", code.move);
}
