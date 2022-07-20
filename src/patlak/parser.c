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

/* Create a reference literal from the next token. */
CTPatlakObject ct_patlak_parser_reference(CTPatlakParserTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_LITERAL_REFERENCE,
        .value = ct_patlak_parser_next(tokens)};
}

/* Create a quoted literal from the next token. */
CTPatlakObject ct_patlak_parser_quote(CTPatlakParserTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_LITERAL_STRING,
        .value = ct_patlak_parser_next(tokens)};
}

/* Create a wildcard literal from the next token. */
CTPatlakObject ct_patlak_parser_wildcard(CTPatlakParserTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_LITERAL_WILDCARD,
        .value = ct_patlak_parser_next(tokens)};
}

/* Create a group from the next token. Does not consume the tokens inside the
 * group and the closing curly bracket! */
CTPatlakObject ct_patlak_parser_group(CTPatlakParserTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_GROUP,
        .value = ct_patlak_parser_next(tokens)};
}

/* Create an object by the given amount of tokens. */
CTPatlakObject ct_patlak_parser_prefix(
    CTPatlakParserTokens* tokens,
    CTPatlakObjectType    type,
    CTIndex               amount)
{
    CTString value = ct_patlak_parser_next(tokens);
    for (CTIndex i = 1; i < amount; i++) {
        value.last = ct_patlak_parser_next(tokens).last;
    }
    return (CTPatlakObject){.type = type, .value = value};
}

/* Create a repeat with one token in or without the square brackets. */
CTPatlakObject ct_patlak_parser_repeat_one(
    CTPatlakParserTokens* tokens,
    CTIndex               amount,
    bool                  bracketed)
{
    switch (ct_patlak_parser_type(tokens, amount)) {
        case CT_PATLAK_TOKEN_NUMBER:
            return ct_patlak_parser_prefix(
                tokens,
                CT_PATLAK_OBJECT_REPEAT_FIXED,
                1 + bracketed * 2);
        case CT_PATLAK_TOKEN_QUESTION_MARK:
            return ct_patlak_parser_prefix(
                tokens,
                CT_PATLAK_OBJECT_REPEAT_RANGE,
                1 + bracketed * 2);
        case CT_PATLAK_TOKEN_STAR:
        case CT_PATLAK_TOKEN_PLUS:
            return ct_patlak_parser_prefix(
                tokens,
                CT_PATLAK_OBJECT_REPEAT_INFINITE,
                1 + bracketed * 2);
        default:
            return (CTPatlakObject){.type = -1};
    }
}

/* Create a repeat with two token in the square brackets. */
CTPatlakObject ct_patlak_parser_repeat_two(CTPatlakParserTokens* tokens)
{
    switch (ct_patlak_parser_type(tokens, 1)) {
        case CT_PATLAK_TOKEN_NUMBER:
            if (ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                return ct_patlak_parser_prefix(
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_INFINITE,
                    4);
            }
            break;
        case CT_PATLAK_TOKEN_COMMA:
            if (ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                return ct_patlak_parser_prefix(
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_RANGE,
                    4);
            }
            break;
        default:
            break;
    }
    ct_unexpected("Unexpected token in the square brackets!");
    return (CTPatlakObject){.type = -1};
}

/* Create a repeat with three token in the square brackets. */
CTPatlakObject ct_patlak_parser_repeat_three(CTPatlakParserTokens* tokens)
{
    ct_expect(
        ct_patlak_parser_type(tokens, 1) == CT_PATLAK_TOKEN_NUMBER &&
            ct_patlak_parser_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA &&
            ct_patlak_parser_type(tokens, 3) == CT_PATLAK_TOKEN_NUMBER,
        "Unexpected token in the square brackets!");
    return ct_patlak_parser_prefix(tokens, CT_PATLAK_OBJECT_REPEAT_RANGE, 5);
}

/* Create a repeat with tokens in the square brackets. */
CTPatlakObject ct_patlak_parser_repeat(CTPatlakParserTokens* tokens)
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
        case 1: {
            CTPatlakObject result =
                ct_patlak_parser_repeat_one(tokens, 1, true);
            ct_expect(
                result.type != (CTPatlakObjectType)-1,
                "Unexpected token in the square brackets!");
            return result;
        }
        case 2:
            return ct_patlak_parser_repeat_two(tokens);
        case 3:
            return ct_patlak_parser_repeat_three(tokens);
        default:
            ct_unexpected("Too many tokens inside the square bracket!");
    }
    return (CTPatlakObject){.type = -1};
}

/* Create an object from the next tokens. */
CTPatlakObject ct_patlak_parser_object(CTPatlakParserTokens* tokens)
{
    switch (ct_patlak_parser_first(tokens)) {
        case CT_PATLAK_TOKEN_IDENTIFIER:
            return ct_patlak_parser_reference(tokens);
        case CT_PATLAK_TOKEN_QUOTE:
            return ct_patlak_parser_quote(tokens);
        case CT_PATLAK_TOKEN_DOT:
            return ct_patlak_parser_wildcard(tokens);
        case CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET:
            return ct_patlak_parser_group(tokens);
        case CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET:
            return ct_patlak_parser_repeat(tokens);
        default:
            return ct_patlak_parser_repeat_one(tokens, 0, false);
    }
}

/* Get the tokens inside the group and consume them. */
CTPatlakParserTokens ct_patlak_parser_group_tokens(CTPatlakParserTokens* tokens)
{
    CTIndex closing_bracket = 1;
    for (CTIndex i = 0; i < ct_patlak_parser_size(tokens); i++) {
        switch (ct_patlak_parser_type(tokens, i)) {
            case CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET:
                closing_bracket++;
                break;
            case CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET:
                closing_bracket--;
                if (closing_bracket == 0) {
                    CTPatlakParserTokens result = {
                        .first = tokens->first,
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

// For use by binary operator.
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens);

/* Try to parse a binary operator. */
void ct_patlak_parser_binary_operator(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens,
    CTPatlakObject        firstObject)
{
    if (ct_patlak_parser_finite(tokens) &&
        ct_patlak_parser_starts(tokens, CT_PATLAK_TOKEN_PIPE)) {
        CTPatlakObject binaryOperator = {
            .type  = CT_PATLAK_OBJECT_OR,
            .value = ct_patlak_parser_next(tokens)};
        ct_patlak_builder_add(builder, binaryOperator);
        ct_patlak_builder_push(builder);
        ct_patlak_builder_add(builder, firstObject);
        ct_patlak_parser_unit(builder, tokens);
    } else {
        ct_patlak_builder_add(builder, firstObject);
    }
}

// For use by unit.
void ct_patlak_parser_units(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens);

/* Parse a single unit into the stack. */
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder*  builder,
    CTPatlakParserTokens* tokens)
{
    CTPatlakObject firstObject = ct_patlak_parser_object(tokens);
    switch (firstObject.type) {
        case CT_PATLAK_OBJECT_LITERAL_REFERENCE:
        case CT_PATLAK_OBJECT_LITERAL_CHARACTER:
        case CT_PATLAK_OBJECT_LITERAL_STRING:
        case CT_PATLAK_OBJECT_LITERAL_RANGE:
        case CT_PATLAK_OBJECT_LITERAL_WILDCARD:
            ct_patlak_parser_binary_operator(builder, tokens, firstObject);
            return;
        case CT_PATLAK_OBJECT_GROUP: {
            CTPatlakParserTokens inside = ct_patlak_parser_group_tokens(tokens);
            firstObject.value.last      = inside.last->value.last;
            ct_patlak_builder_add(builder, firstObject);
            ct_patlak_parser_units(builder, &inside);
            return;
        }
        case CT_PATLAK_OBJECT_REPEAT_FIXED:
        case CT_PATLAK_OBJECT_REPEAT_RANGE:
        case CT_PATLAK_OBJECT_REPEAT_INFINITE:
            ct_patlak_builder_add(builder, firstObject);
            ct_patlak_builder_push(builder);
            ct_patlak_parser_unit(builder, tokens);
            ct_patlak_builder_pop(builder);
            return;
        default:
            // DEBUG: Print unexpected token.
            printf("Unexpected Token: \"");
            ct_patlak_printer_token(tokens->first);
            printf("\"\n");
            ct_unexpected("Unexpected token!");
    }
}

/* Parse the all the avalible units in the group by joining them with ands. */
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
