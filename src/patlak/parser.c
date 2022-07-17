// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/builder.c"
#include "patlak/object.c"
#include "patlak/printer.c"
#include "patlak/token.c"
#include "prelude/expect.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

#include <stdio.h>

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

/* Type of the token after the next one by the given amount. */
CTPatlakTokenType
ct_patlak_parser_type(CTPatlakParserTokens const* tokens, CTIndex amount)
{
    return tokens->first[amount].type;
}

/* Type of the next token. */
CTPatlakTokenType ct_patlak_parser_first(CTPatlakParserTokens const* tokens)
{
    return ct_patlak_parser_type(tokens, 0);
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

/* Parse the next reference literal. */
void ct_patlak_parser_reference(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    ct_patlak_builder_add(
        builder,
        (CTPatlakObject){
            .type  = CT_PATLAK_OBJECT_LITERAL_REFERENCE,
            .value = ct_patlak_parser_next(tokens)});
}

/* Parse the next quote token. */
void ct_patlak_parser_quote(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    ct_patlak_builder_add(
        builder,
        (CTPatlakObject){
            .type  = CT_PATLAK_OBJECT_LITERAL_STRING,
            .value = ct_patlak_parser_next(tokens)});
}

/* Parse the next wildcard literal. */
void ct_patlak_parser_wildcard(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    ct_patlak_builder_add(
        builder,
        (CTPatlakObject){
            .type  = CT_PATLAK_OBJECT_LITERAL_WILDCARD,
            .value = ct_patlak_parser_next(tokens)});
}

// For use in prefix.
void ct_patlak_parser_binary_operators(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakTokenType     until);

/* Parse units that are formed by prefixing another unit with the given amount
 * of tokens. */
void ct_patlak_parser_prefix(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakObjectType    type,
    CTIndex               amount,
    CTPatlakTokenType     until)
{
    CTString value = ct_patlak_parser_next(tokens);
    for (CTIndex i = 1; i < amount; i++) {
        value.last = ct_patlak_parser_next(tokens).last;
    }
    ct_patlak_builder_add(
        builder,
        (CTPatlakObject){.type = type, .value = value});
    ct_patlak_builder_push(builder);
    ct_patlak_parser_binary_operators(builder, tokens, until);
    ct_patlak_builder_pop(builder);
}

/* Parse the next group unit. */
void ct_patlak_parser_group(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    ct_patlak_parser_prefix(
        builder,
        tokens,
        CT_PATLAK_OBJECT_GROUP,
        1,
        CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET);
    ct_expect(
        ct_patlak_parser_starts(tokens, CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET),
        "No closing curly bracket!");
    ct_patlak_parser_next(tokens);
}

/* Parse repeat units with one token in the square brackets. */
bool ct_patlak_parser_repeat_one(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTIndex               amount,
    bool                  bracketed,
    CTPatlakTokenType     until)
{
    switch (ct_patlak_parser_type(tokens, amount)) {
        case CT_PATLAK_TOKEN_NUMBER:
            ct_patlak_parser_prefix(
                builder,
                tokens,
                CT_PATLAK_OBJECT_REPEAT_FIXED,
                1 + bracketed * 2,
                until);
            return true;
        case CT_PATLAK_TOKEN_QUESTION_MARK:
            ct_patlak_parser_prefix(
                builder,
                tokens,
                CT_PATLAK_OBJECT_REPEAT_RANGE,
                1 + bracketed * 2,
                until);
            return true;
        case CT_PATLAK_TOKEN_STAR:
        case CT_PATLAK_TOKEN_PLUS:
            ct_patlak_parser_prefix(
                builder,
                tokens,
                CT_PATLAK_OBJECT_REPEAT_INFINITE,
                1 + bracketed * 2,
                until);
            return true;
        default:
            return false;
    }
}

/* Parse repeat units with two tokens in the square brackets. */
void ct_patlak_parser_repeat_two(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakTokenType     until)
{
    switch (ct_patlak_parser_type(tokens, 1)) {
        case CT_PATLAK_TOKEN_NUMBER:
            if (ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                ct_patlak_parser_prefix(
                    builder,
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_INFINITE,
                    4,
                    until);
                return;
            }
            break;
        case CT_PATLAK_TOKEN_COMMA:
            if (ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                ct_patlak_parser_prefix(
                    builder,
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_RANGE,
                    4,
                    until);
                return;
            }
            break;
        default:
            break;
    }
    ct_unexpected("Unexpected token in the square brackets!");
}

/* Parse repeat units with three tokens in the square brackets. */
void ct_patlak_parser_repeat_three(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakTokenType     until)
{
    ct_expect(
        ct_patlak_parser_type(tokens, 1) == CT_PATLAK_TOKEN_NUMBER &&
            ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA &&
            ct_patlak_parser_type(tokens, 3) == CT_PATLAK_TOKEN_NUMBER,
        "Unexpected token in the square brackets!");
    ct_patlak_parser_prefix(
        builder,
        tokens,
        CT_PATLAK_OBJECT_REPEAT_RANGE,
        5,
        until);
}

/* Parse the next repeat unit. */
void ct_patlak_parser_repeat(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakTokenType     until)
{
    CTIndex avalible = ct_patlak_parser_size(tokens);
    ct_expect(
        avalible >= 3,
        "Not enough tokens after opening square brackets!");
    CTIndex insideBrackets = -1;
    for (CTIndex i = 1; i < avalible; i++) {
        if (ct_patlak_parser_type(tokens, i) ==
            CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET) {
            insideBrackets = i - 1;
        }
    }
    switch (insideBrackets) {
        case -1:
            ct_unexpected("No closing square bracket!");
        case 0:
            ct_unexpected("No tokens inside the square bracket!");
        case 1:
            ct_expect(
                ct_patlak_parser_repeat_one(builder, tokens, 1, true, until),
                "Unexpected token in the square brackets!");
            return;
        case 2:
            ct_patlak_parser_repeat_two(builder, tokens, until);
            return;
        case 3:
            ct_patlak_parser_repeat_three(builder, tokens, until);
            return;
        default:
            ct_unexpected("Too many tokens inside the square bracket!");
    }
}

/* Parse the next unit. */
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakTokenType     until)
{
    ct_expect(ct_patlak_parser_finite(tokens), "Pattern ends unexpectedly!");
    // DEBUG: Print the token.
    printf("Parsing Token: \"");
    ct_patlak_printer_token(tokens->first);
    printf("\"\n");
    switch (ct_patlak_parser_first(tokens)) {
        case CT_PATLAK_TOKEN_IDENTIFIER:
            ct_patlak_parser_reference(builder, tokens);
            return;
        case CT_PATLAK_TOKEN_QUOTE:
            ct_patlak_parser_quote(builder, tokens);
            return;
        case CT_PATLAK_TOKEN_DOT:
            ct_patlak_parser_wildcard(builder, tokens);
            return;
        case CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET:
            ct_patlak_parser_group(builder, tokens);
            return;
        case CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET:
            ct_patlak_parser_repeat(builder, tokens, until);
            return;
        default:
            if (ct_patlak_parser_repeat_one(builder, tokens, 0, false, until)) {
                return;
            }
            break;
    }
    // DEBUG: Print unexpected token.
    printf("Unexpected Token: \"");
    ct_patlak_printer_token(tokens->first);
    printf("\"\n");
    ct_unexpected("Unexpected token!");
}

/* Parse the next units with binary operators in mind. */
void ct_patlak_parser_binary_operators(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakTokenType     until)
{
    CTPatlakTree        subtree    = {0};
    CTPatlakTreeBuilder subbuilder = {.tree = &subtree};

    while (ct_patlak_parser_finite(tokens)) {
        ct_patlak_parser_unit(&subbuilder, tokens, until);
        if (ct_patlak_parser_starts(tokens, until) ||
            !ct_patlak_parser_finite(tokens)) {
            ct_patlak_builder_add_subtree(builder, subtree.first);
            break;
        }
        if (ct_patlak_parser_starts(tokens, CT_PATLAK_TOKEN_PIPE)) {
            ct_patlak_builder_add(
                builder,
                (CTPatlakObject){
                    .type  = CT_PATLAK_OBJECT_OR,
                    .value = ct_patlak_parser_next(tokens)});
            ct_patlak_builder_add_subtree(builder, subtree.first);
            ct_patlak_parser_unit(builder, tokens, until);
        } else {
            ct_patlak_builder_add(
                builder,
                (CTPatlakObject){.type = CT_PATLAK_OBJECT_AND});
            ct_patlak_builder_add_subtree(builder, subtree.first);
            ct_patlak_parser_unit(builder, tokens, until);
        }
    }

    ct_patlak_builder_free(&subbuilder);
    ct_patlak_tree_free(&subtree);
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
    ct_patlak_parser_binary_operators(&builder, &remaining, -1);
    ct_patlak_builder_free(&builder);
}
