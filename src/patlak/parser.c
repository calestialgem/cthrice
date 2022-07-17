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

/* Get the tokens inside the group and consume them. */
CTPatlakParserTokens ct_patlak_parser_group_tokens(CTPatlakParserTokens* tokens)
{
    CTIndex closing_bracket = 1;
    for (CTIndex i = 1; i < ct_patlak_parser_size(tokens); i++) {
        switch (ct_patlak_parser_type(tokens, i)) {
            case CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET:
                closing_bracket++;
                break;
            case CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET:
                closing_bracket--;
                if (closing_bracket == 0) {
                    CTPatlakParserTokens result = {
                        .first = tokens->first + 1,
                        .last  = tokens->first + i};
                    tokens->first += i + 1;
                    return result;
                }
            default:
                break;
        }
    }
    ct_unexpected("No closing curly bracket!");
    return (CTPatlakParserTokens){0};
}

// For recursing from the group.
void ct_patlak_parser_units(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens);

/* Parse the next group unit. */
void ct_patlak_parser_group(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    CTPatlakParserTokens inside = ct_patlak_parser_group_tokens(tokens);
    ct_patlak_builder_add(
        builder,
        (CTPatlakObject){
            .type  = CT_PATLAK_OBJECT_GROUP,
            .value = {
                      .first = (inside.first - 1)->value.first,
                      .last  = inside.last->value.last}
    });
    ct_patlak_parser_units(builder, &inside);
}

// For use in prefix.
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens);

/* Parse units that are formed by prefixing another unit with the given amount
 * of tokens. */
void ct_patlak_parser_prefix(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakObjectType    type,
    CTIndex               amount)
{
    CTString value = ct_patlak_parser_next(tokens);
    for (CTIndex i = 1; i < amount; i++) {
        value.last = ct_patlak_parser_next(tokens).last;
    }
    ct_patlak_builder_add(
        builder,
        (CTPatlakObject){.type = type, .value = value});
    ct_patlak_builder_push(builder);
    ct_patlak_parser_unit(builder, tokens);
    ct_patlak_builder_pop(builder);
}

/* Parse repeat units with one token in the square brackets. */
bool ct_patlak_parser_repeat_one(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTIndex               amount,
    bool                  bracketed)
{
    switch (ct_patlak_parser_type(tokens, amount)) {
        case CT_PATLAK_TOKEN_NUMBER:
            ct_patlak_parser_prefix(
                builder,
                tokens,
                CT_PATLAK_OBJECT_REPEAT_FIXED,
                1 + bracketed * 2);
            return true;
        case CT_PATLAK_TOKEN_QUESTION_MARK:
            ct_patlak_parser_prefix(
                builder,
                tokens,
                CT_PATLAK_OBJECT_REPEAT_RANGE,
                1 + bracketed * 2);
            return true;
        case CT_PATLAK_TOKEN_STAR:
        case CT_PATLAK_TOKEN_PLUS:
            ct_patlak_parser_prefix(
                builder,
                tokens,
                CT_PATLAK_OBJECT_REPEAT_INFINITE,
                1 + bracketed * 2);
            return true;
        default:
            return false;
    }
}

/* Parse repeat units with two tokens in the square brackets. */
void ct_patlak_parser_repeat_two(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    switch (ct_patlak_parser_type(tokens, 1)) {
        case CT_PATLAK_TOKEN_NUMBER:
            if (ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                ct_patlak_parser_prefix(
                    builder,
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_INFINITE,
                    4);
                return;
            }
            break;
        case CT_PATLAK_TOKEN_COMMA:
            if (ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                ct_patlak_parser_prefix(
                    builder,
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_RANGE,
                    4);
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
    CTPatlakParserTokens* tokens)
{
    ct_expect(
        ct_patlak_parser_type(tokens, 1) == CT_PATLAK_TOKEN_NUMBER &&
            ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA &&
            ct_patlak_parser_type(tokens, 3) == CT_PATLAK_TOKEN_NUMBER,
        "Unexpected token in the square brackets!");
    ct_patlak_parser_prefix(builder, tokens, CT_PATLAK_OBJECT_REPEAT_RANGE, 5);
}

/* Parse the next repeat unit. */
void ct_patlak_parser_repeat(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
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
            break;
        }
    }
    switch (insideBrackets) {
        case -1:
            ct_unexpected("No closing square bracket!");
        case 0:
            ct_unexpected("No tokens inside the square bracket!");
        case 1:
            ct_expect(
                ct_patlak_parser_repeat_one(builder, tokens, 1, true),
                "Unexpected token in the square brackets!");
            return;
        case 2:
            ct_patlak_parser_repeat_two(builder, tokens);
            return;
        case 3:
            ct_patlak_parser_repeat_three(builder, tokens);
            return;
        default:
            ct_unexpected("Too many tokens inside the square bracket!");
    }
}

/* Parse a single unit into the stack. */
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
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
            ct_patlak_parser_repeat(builder, tokens);
            return;
        default:
            if (ct_patlak_parser_repeat_one(builder, tokens, 0, false)) {
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

/* Parse the all the avalible units. */
void ct_patlak_parser_units(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    ct_patlak_builder_push(builder);
    while (ct_patlak_parser_finite(tokens)) {
        ct_patlak_parser_unit(builder, tokens);
    }
    ct_patlak_builder_pop(builder);
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
    ct_patlak_builder_add_root(&builder, definition);
    ct_patlak_builder_add(&builder, decleration);

    // Parse all the units into an implicit group.
    ct_patlak_builder_add(
        &builder,
        (CTPatlakObject){
            .type  = CT_PATLAK_OBJECT_GROUP,
            .value = {
                      .first = remaining.first->value.first,
                      .last  = remaining.last->value.last}
    });
    ct_patlak_parser_units(&builder, &remaining);
    ct_patlak_builder_free(&builder);
}
