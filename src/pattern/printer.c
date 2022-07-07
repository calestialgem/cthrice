// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "internal.h"
#include "string/api.h"

#include <stdio.h>

/* Format string from the token type. */
static byte* token_type_name(enum ptrntkntyp typ)
{
    switch (typ) {
        case TOKEN_EQUAL:
            return "= ";
        case TOKEN_DOT:
            return ". ";
        case TOKEN_PIPE:
            return "| ";
        case TOKEN_COMMA:
            return ", ";
        case TOKEN_QUESTION_MARK:
            return "? ";
        case TOKEN_STAR:
            return "* ";
        case TOKEN_PLUS:
            return "+ ";
        case TOKEN_OPENING_CURLY_BRACKET:
            return "{ ";
        case TOKEN_CLOSING_CURLY_BRACKET:
            return "} ";
        case TOKEN_OPENING_SQUARE_BRACKET:
            return "[ ";
        case TOKEN_CLOSING_SQUARE_BRACKET:
            return "] ";
        case TOKEN_NUMBER:
            return "number {%.*s} ";
        case TOKEN_QUOTE:
            return "quote {%.*s} ";
        case TOKEN_IDENTIFIER:
            return "identifier {%.*s} ";
        default:
            return "unknown {%.*s} ";
    }
}

void ptrn_print_lex(struct ptrnlex lex)
{
    printf("\n  LEX\n=======\n\n");
    for (const struct ptrntkn* i = lex.bgn; i < lex.end; i++) {
        printf(token_type_name(i->typ), (int)str_size(i->val), i->val.bgn);
    }
    printf("\n");
}

void ptrn_print_infos(struct ptrnctx ctx)
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

void ptrn_print_codes(struct ptrnctx ctx)
{
    printf("\n  PATTERN CODE\n================\n\n");
    for (const struct ptrncode* i = ctx.code.bgn; i < ctx.code.end; i++) {
        printf("[%05lld] ", i - ctx.code.bgn);
        ptrn_print_code(*i);
    }
}

void ptrn_print_code(struct ptrncode code)
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
