// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/builder.c"
#include "patlak/object.c"
#include "patlak/token.c"
#include "prelude/expect.c"
#include "prelude/scalar.c"

/* Immutable view of tokens. */
typedef struct {
    /* Border before the first token. */
    CTPatlakToken const* first;
    /* Border after the last token. */
    CTPatlakToken const* last;
} CTPatlakParserTokens;

/* Amount of tokens. */
CTIndex ct_patlak_parser_size(CTPatlakParserTokens const* tokens)
{
    return tokens->last - tokens->first;
}

/* Whether there are any tokens. */
bool ct_patlak_parser_finite(CTPatlakParserTokens const* tokens)
{
    return ct_patlak_parser_size(tokens) > 0;
}

/* Type of the next token. */
CTPatlakTokenType ct_patlak_parser_first(CTPatlakParserTokens const* tokens)
{
    return tokens->first->type;
}

/* Whether the next token is of the given type. */
bool ct_patlak_parser_starts(
    CTPatlakParserTokens const* tokens,
    CTPatlakTokenType           type)
{
    return ct_patlak_parser_first(tokens) == type;
}

/* Consume the next token and return its value. */
CTString ct_patlak_parser_next(CTPatlakParserTokens* tokens)
{
    return tokens->first++->value;
}

/* Parse the next unit. */
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    ct_expect(ct_patlak_parser_finite(tokens), "Pattern ends unexpectedly!");
    switch (ct_patlak_parser_first(tokens)) {
        case CT_PATLAK_TOKEN_DOT:
            ct_patlak_builder_add(
                builder,
                (CTPatlakObject){
                    .type  = CT_PATLAK_OBJECT_LITERAL_WILDCARD,
                    .value = ct_patlak_parser_next(tokens)});
            return;
        case CT_PATLAK_TOKEN_QUOTE:
            ct_patlak_builder_add(
                builder,
                (CTPatlakObject){
                    .type  = CT_PATLAK_OBJECT_LITERAL_STRING,
                    .value = ct_patlak_parser_next(tokens)});
            return;
        case CT_PATLAK_TOKEN_IDENTIFIER:
            ct_patlak_builder_add(
                builder,
                (CTPatlakObject){
                    .type  = CT_PATLAK_OBJECT_LITERAL_REFERENCE,
                    .value = ct_patlak_parser_next(tokens)});
            return;
        default:
            ct_expect(false, "Unexpected token!");
    }
}

/* Parse the tokens. */
void ct_patlak_parser(CTPatlakTree* tree, CTPatlakTokens const* tokens)
{
    CTPatlakTreeBuilder  builder   = {.tree = tree};
    CTPatlakParserTokens remaining = {
        .first = tokens->first,
        .last  = tokens->last};

    // Parse the name of the pattern.
    ct_expect(
        ct_patlak_parser_finite(&remaining) &&
            ct_patlak_parser_starts(&remaining, CT_PATLAK_TOKEN_IDENTIFIER),
        "There is no pattern name!");
    CTPatlakObject decleration = {
        .type  = CT_PATLAK_OBJECT_DECLERATION,
        .value = ct_patlak_parser_next(&remaining)};

    // Parse the equal sign.
    ct_expect(
        ct_patlak_parser_finite(&remaining) &&
            ct_patlak_parser_starts(&remaining, CT_PATLAK_TOKEN_EQUAL),
        "There is no pattern definition!");
    CTPatlakObject definition = {
        .type  = CT_PATLAK_OBJECT_DEFINITION,
        .value = ct_patlak_parser_next(&remaining)};

    // Create the root of the tree.
    ct_patlak_builder_add(&builder, definition);
    ct_patlak_builder_push(&builder);
    ct_patlak_builder_add(&builder, decleration);
    ct_patlak_parser_unit(&builder, &remaining);
    ct_patlak_builder_free(&builder);
}
