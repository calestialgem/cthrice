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

/* Create a reference literal from the next token. */
CTPatlakObject ct_patlak_parser_reference(CTPatlakTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_LITERAL_REFERENCE,
        .value = ct_patlak_tokens_next(tokens)};
}

/* Create a quoted literal from the next token. */
CTPatlakObject ct_patlak_parser_quote(CTPatlakTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_LITERAL_STRING,
        .value = ct_patlak_tokens_next(tokens)};
}

/* Create a wildcard literal from the next token. */
CTPatlakObject ct_patlak_parser_wildcard(CTPatlakTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_LITERAL_WILDCARD,
        .value = ct_patlak_tokens_next(tokens)};
}

/* Create a group from the next token. Does not consume the tokens inside the
 * group and the closing curly bracket! */
CTPatlakObject ct_patlak_parser_group(CTPatlakTokens* tokens)
{
    return (CTPatlakObject){
        .type  = CT_PATLAK_OBJECT_GROUP,
        .value = ct_patlak_tokens_next(tokens)};
}

/* Create an object by the given amount of tokens. */
CTPatlakObject ct_patlak_parser_prefix(
    CTPatlakTokens*    tokens,
    CTPatlakObjectType type,
    CTIndex            amount)
{
    return (CTPatlakObject){
        .type  = type,
        .value = ct_patlak_tokens_nexts(tokens, amount)};
}

/* Create a repeat with one token in or without the square brackets. */
CTPatlakObject ct_patlak_parser_repeat_one(
    CTPatlakTokens* tokens,
    CTIndex         amount,
    bool            bracketed)
{
    switch (ct_patlak_tokens_type(tokens, amount)) {
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
CTPatlakObject ct_patlak_parser_repeat_two(CTPatlakTokens* tokens)
{
    switch (ct_patlak_tokens_type(tokens, 1)) {
        case CT_PATLAK_TOKEN_NUMBER:
            if (ct_patlak_tokens_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
                return ct_patlak_parser_prefix(
                    tokens,
                    CT_PATLAK_OBJECT_REPEAT_INFINITE,
                    4);
            }
            break;
        case CT_PATLAK_TOKEN_COMMA:
            if (ct_patlak_tokens_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA) {
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
CTPatlakObject ct_patlak_parser_repeat_three(CTPatlakTokens* tokens)
{
    ct_expect(
        ct_patlak_tokens_type(tokens, 1) == CT_PATLAK_TOKEN_NUMBER &&
            ct_patlak_tokens_type(tokens, 2) == CT_PATLAK_TOKEN_COMMA &&
            ct_patlak_tokens_type(tokens, 3) == CT_PATLAK_TOKEN_NUMBER,
        "Unexpected token in the square brackets!");
    return ct_patlak_parser_prefix(tokens, CT_PATLAK_OBJECT_REPEAT_RANGE, 5);
}

/* Create a repeat with tokens in the square brackets. */
CTPatlakObject ct_patlak_parser_repeat(CTPatlakTokens* tokens)
{
    CTIndex avalible = ct_patlak_tokens_size(tokens);
    ct_expect(
        avalible >= 3,
        "Not enough tokens after opening square brackets!");
    CTIndex insideBrackets = -1;
    for (CTIndex i = 1; i < avalible; i++) {
        if (ct_patlak_tokens_type(tokens, i) ==
            CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET) {
            insideBrackets = i - 1;
            break;
        }
    }
    switch (insideBrackets) {
        case -1:
            ct_unexpected("No closing square bracket!");
            break;
        case 0:
            ct_unexpected("No tokens inside the square bracket!");
            break;
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
CTPatlakObject ct_patlak_parser_object(CTPatlakTokens* tokens)
{
    switch (ct_patlak_tokens_first(tokens)) {
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

/* Get the tokens inside the brackets and consume them. */
CTPatlakTokens ct_patlak_parser_bracket(
    CTPatlakTokens*   tokens,
    CTPatlakTokenType opening,
    CTPatlakTokenType closing)
{
    CTIndex closing_bracket = 1;
    for (CTIndex i = 0; i < ct_patlak_tokens_size(tokens); i++) {
        CTPatlakTokenType type = ct_patlak_tokens_type(tokens, i);
        if (type == opening) {
            closing_bracket++;
        } else if (type == closing) {
            closing_bracket--;
            if (closing_bracket == 0) {
                CTPatlakTokens result = {
                    .first = tokens->first,
                    .last  = tokens->first + i};
                tokens->first += i + 1;
                return result;
            }
        }
    }
    ct_unexpected("No closing bracket!");
    return (CTPatlakTokens){0};
}

// For use by afterwards.
void ct_patlak_parser_unit(
    CTPatlakTreeBuilder* builder,
    CTPatlakTokens*      tokens);

// For use by afterwards and unit.
void ct_patlak_parser_units(
    CTPatlakTreeBuilder* builder,
    CTPatlakTokens*      tokens);

/* Try to parse an object that looks like a simpler object. They differ by the
 * token that comes after the token that led to the creation of the first
 * object. */
void ct_patlak_parser_afterwards(
    CTPatlakTreeBuilder* builder,
    CTPatlakTokens*      tokens,
    CTPatlakObject       firstObject)
{
    if (ct_patlak_tokens_finite(tokens)) {
        switch (ct_patlak_tokens_first(tokens)) {
            case CT_PATLAK_TOKEN_PIPE: {
                CTPatlakObject binaryOperator = {
                    .type  = CT_PATLAK_OBJECT_OR,
                    .value = ct_patlak_tokens_next(tokens)};
                ct_patlak_builder_add(builder, binaryOperator);
                ct_patlak_builder_push(builder);
                ct_patlak_builder_add(builder, firstObject);
                ct_patlak_parser_unit(builder, tokens);
                ct_patlak_builder_pop(builder);
                return;
            }
            case CT_PATLAK_TOKEN_OPENING_BRACKET: {
                ct_expect(
                    firstObject.type == CT_PATLAK_OBJECT_LITERAL_REFERENCE,
                    "Function call argument list after something that is not a "
                    "function name!");
                firstObject.type = CT_PATLAK_OBJECT_FUNCTION_CALL;
                // Skip opening brackets.
                tokens->first++;
                CTPatlakTokens inside = ct_patlak_parser_bracket(
                    tokens,
                    CT_PATLAK_TOKEN_OPENING_BRACKET,
                    CT_PATLAK_TOKEN_CLOSING_BRACKET);
                ct_patlak_builder_add(builder, firstObject);
                ct_patlak_builder_push(builder);
                while (ct_patlak_tokens_finite(&inside)) {
                    CTPatlakTokens argument = {
                        .first = inside.first,
                        .last  = inside.first};
                    while (argument.last < inside.last) {
                        if (argument.last->type == CT_PATLAK_TOKEN_COMMA) {
                            break;
                        }
                        argument.last++;
                    }
                    inside.first = argument.last + 1;
                    // Parse the argument to an implicit group.
                    ct_patlak_builder_add(
                        builder,
                        (CTPatlakObject){
                            .type  = CT_PATLAK_OBJECT_GROUP,
                            .value = argument});
                    ct_patlak_parser_units(builder, &argument);
                }
                ct_patlak_builder_pop(builder);
                return;
            }
            default:
                break;
        }
    }
    // If there are no tokens or the next token does not makes a bigger object:
    // the first object is the correct outcome.
    ct_patlak_builder_add(builder, firstObject);
}

/* Parse a single unit into the stack. */
void ct_patlak_parser_unit(CTPatlakTreeBuilder* builder, CTPatlakTokens* tokens)
{
    CTPatlakObject firstObject = ct_patlak_parser_object(tokens);
    switch (firstObject.type) {
        case CT_PATLAK_OBJECT_LITERAL_REFERENCE:
        case CT_PATLAK_OBJECT_LITERAL_CHARACTER:
        case CT_PATLAK_OBJECT_LITERAL_STRING:
        case CT_PATLAK_OBJECT_LITERAL_RANGE:
        case CT_PATLAK_OBJECT_LITERAL_WILDCARD:
            ct_patlak_parser_afterwards(builder, tokens, firstObject);
            return;
        case CT_PATLAK_OBJECT_GROUP: {
            CTPatlakTokens inside = ct_patlak_parser_bracket(
                tokens,
                CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET,
                CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET);
            firstObject.value.last = inside.last;
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
            ct_unexpected("Unexpected token in the pattern definition!");
    }
}

/* Parse the all the avalible units in the group by joining them with ands. */
void ct_patlak_parser_units(
    CTPatlakTreeBuilder* builder,
    CTPatlakTokens*      tokens)
{
    ct_patlak_builder_push(builder);
    while (ct_patlak_tokens_finite(tokens)) {
        ct_patlak_parser_unit(builder, tokens);
    }
    ct_patlak_builder_pop(builder);
}

/* Parse the parameters in the decleration of pattern function. */
CTPatlakTokens ct_patlak_parser_parameters(CTPatlakTokens* tokens)
{
    if (!ct_patlak_tokens_finite(tokens) ||
        !ct_patlak_tokens_starts(tokens, CT_PATLAK_TOKEN_OPENING_BRACKET)) {
        return (CTPatlakTokens){0};
    }
    // Consume the opening bracket.
    tokens->first++;
    return ct_patlak_parser_bracket(
        tokens,
        CT_PATLAK_TOKEN_OPENING_BRACKET,
        CT_PATLAK_TOKEN_CLOSING_BRACKET);
}

/* Parse the decleration part of pattern definition. */
void ct_patlak_parser_decleration(
    CTPatlakTreeBuilder* builder,
    CTPatlakTokens*      tokens)
{
    // Parse the name.
    ct_expect(
        ct_patlak_tokens_finite(tokens) &&
            ct_patlak_tokens_starts(tokens, CT_PATLAK_TOKEN_IDENTIFIER),
        "There is no pattern name!");
    CTPatlakObject decleration = {
        .type  = CT_PATLAK_OBJECT_DECLERATION,
        .value = ct_patlak_tokens_next(tokens)};

    // Get the parameter tokens.
    CTPatlakTokens parameters = ct_patlak_parser_parameters(tokens);

    // Parse the equal sign.
    ct_expect(
        ct_patlak_tokens_finite(tokens) &&
            ct_patlak_tokens_starts(tokens, CT_PATLAK_TOKEN_EQUAL),
        "There is no pattern definition!");
    CTPatlakObject definition = {
        .type  = CT_PATLAK_OBJECT_DEFINITION,
        .value = ct_patlak_tokens_next(tokens)};

    // Create the root of the tree.
    ct_patlak_builder_add_root(builder, definition);
    ct_patlak_builder_add(builder, decleration);

    // Parse the parameter names.
    ct_patlak_builder_push(builder);
    while (ct_patlak_tokens_finite(&parameters)) {
        ct_expect(
            ct_patlak_tokens_starts(&parameters, CT_PATLAK_TOKEN_IDENTIFIER),
            "Expected parameter name in pattern function decleration!");
        ct_patlak_builder_add(
            builder,
            (CTPatlakObject){
                .type  = CT_PATLAK_OBJECT_PARAMETER,
                .value = ct_patlak_tokens_next(&parameters)});
        if (ct_patlak_tokens_finite(&parameters)) {
            ct_expect(
                ct_patlak_tokens_starts(&parameters, CT_PATLAK_TOKEN_COMMA),
                "Expected a comma between parameter names in the "
                "pattern function decleration!");
            parameters.first++;
        }
    }
    ct_patlak_builder_pop(builder);
}

/* Parse the tokens. */
void ct_patlak_parser(CTPatlakTree* tree, CTPatlakTokenList const* tokens)
{
    CTPatlakTreeBuilder builder = {.tree = tree};
    CTPatlakTokens remaining = {.first = tokens->first, .last = tokens->last};

    // Parse the decleration.
    ct_patlak_parser_decleration(&builder, &remaining);

    // Parse all the units into an implicit group.
    ct_patlak_builder_add(
        &builder,
        (CTPatlakObject){.type = CT_PATLAK_OBJECT_GROUP, .value = remaining});
    ct_patlak_parser_units(&builder, &remaining);
    ct_patlak_builder_free(&builder);
}
