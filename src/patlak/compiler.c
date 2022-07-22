// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "patlak/object.c"
#include "patlak/pattern.c"
#include "patlak/printer.c"
#include "prelude/expect.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

#include <stdio.h>

// For use by unit compilers.
void ct_patlak_compiler_object(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index);

/* Compile the reference literal. */
void ct_patlak_compiler_reference(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
}

/* Compile the character literal. */
void ct_patlak_compiler_character(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
}

/* Compile the string literal. */
void ct_patlak_compiler_string(CTPatlakCodes* codes, CTPatlakNode const* node)
{
    CTPatlakTokens inside = node->object.value;
    CTString       joined = ct_patlak_tokens_join(&inside);
    ct_patlak_string_unwrap(&joined, '\'', '\'');
    for (char const* i = joined.first; i < joined.last; i++) {
        ct_patlak_codes_add(
            codes,
            (CTPatlakCode){
                .movement = 1,
                .type     = CT_PATLAK_CODE_LITERAL,
                .literal  = *i});
    }
}

/* Compile the character range literal. */
void ct_patlak_compiler_range(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
}

/* Compile the wildcard literal. */
void ct_patlak_compiler_wildcard(CTPatlakCodes* codes)
{
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.movement = 1, .type = CT_PATLAK_CODE_EMPTY});
}

/* Compile the OR binary operator. */
void ct_patlak_compiler_or(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){
            .type     = CT_PATLAK_CODE_BRANCH,
            .branches = node->childeren});
    CTIndex branchStart = ct_patlak_codes_size(codes);
    for (CTIndex i = 0; i < node->childeren; i++) {
        ct_patlak_codes_add(
            codes,
            (CTPatlakCode){.type = CT_PATLAK_CODE_EMPTY});
    }
    for (CTIndex i = 0; i < node->childeren; i++) {
        CTIndex start = ct_patlak_codes_size(codes);
        ct_patlak_compiler_object(codes, patterns, tree, index + 1 + i);
        CTIndex branch                               = branchStart + i;
        ct_patlak_codes_get(codes, branch)->movement = start - branch;
    }
    CTIndex destination = ct_patlak_codes_size(codes);
    for (CTIndex i = 1; i < node->childeren; i++) {
        CTIndex branch = branchStart + i;
        CTIndex caseStart =
            branch + ct_patlak_codes_get(codes, branch)->movement;
        CTIndex caseEnd                               = caseStart - 1;
        ct_patlak_codes_get(codes, caseEnd)->movement = destination - caseEnd;
    }
}

/* Compile the group. */
void ct_patlak_compiler_group(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
    for (CTIndex i = 0; i < node->childeren; i++) {
        ct_patlak_compiler_object(codes, patterns, tree, index + 1 + i);
    }
}

/* Compile the fixed repeat. */
void ct_patlak_compiler_repeat_fixed(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
    // Remove brackets.
    CTPatlakTokens inside = node->object.value;
    ct_patlak_tokens_unwrap(
        &inside,
        CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET,
        CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET);
    ct_expect(
        ct_patlak_tokens_size(&inside) > 0,
        "No tokens in the fixed repeat!");
    ct_expect(
        ct_patlak_tokens_size(&inside) == 1,
        "Too many tokens in the fixed repeat!");
    ct_expect(
        ct_patlak_tokens_starts(&inside, CT_PATLAK_TOKEN_NUMBER),
        "Expected a number in the fixed repeat!");
    unsigned long long number = ct_string_parse(&inside.first->value);
    for (unsigned long long i = 0; i < number; i++) {
        ct_patlak_compiler_object(codes, patterns, tree, index + 1);
    }
}

/* Compile the ranged repeat. */
void ct_patlak_compiler_repeat_range(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
    // Remove brackets.
    CTPatlakTokens inside = node->object.value;
    ct_patlak_tokens_unwrap(
        &inside,
        CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET,
        CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET);
    ct_expect(
        ct_patlak_tokens_size(&inside) > 0,
        "No tokens in the ranged repeat!");
    CTIndex lower = 0;
    CTIndex upper = 1;
    switch (ct_patlak_tokens_first(&inside)) {
        case CT_PATLAK_TOKEN_NUMBER:
            ct_expect(
                ct_patlak_tokens_size(&inside) == 3,
                "Expected two more tokens after number in the ranged repeat!");
            ct_expect(
                ct_patlak_tokens_type(&inside, 1) == CT_PATLAK_TOKEN_COMMA,
                "Expected a comma between numbers in the ranged repeat!");
            ct_expect(
                ct_patlak_tokens_type(&inside, 2) == CT_PATLAK_TOKEN_NUMBER,
                "Expected a number after comma in the ranged repeat!");
            lower = (CTIndex)ct_string_parse(&inside.first[0].value);
            upper = (CTIndex)ct_string_parse(&inside.first[2].value);
            break;

        case CT_PATLAK_TOKEN_COMMA:
            ct_expect(
                ct_patlak_tokens_size(&inside) == 2,
                "Expected one more token after comma in the ranged repeat!");
            ct_expect(
                ct_patlak_tokens_type(&inside, 1) == CT_PATLAK_TOKEN_NUMBER,
                "Expected a number after comma in the ranged repeat!");
            upper = (CTIndex)ct_string_parse(&inside.first[1].value);
            break;

        case CT_PATLAK_TOKEN_QUESTION_MARK:
            ct_expect(
                ct_patlak_tokens_size(&inside) == 1,
                "Expected no more tokens after question mark in the ranged "
                "repeat!");
            break;

        default:
            ct_unexpected("Unexpected token in the ranged repeat!");
    }
    for (CTIndex i = 0; i < lower; i++) {
        ct_patlak_compiler_object(codes, patterns, tree, index + 1);
    }
    CTIndex window = upper - lower;
    ct_expect(
        window > 0,
        "Upper bound must be bigger than the lower bound in the ranged "
        "repeat!");
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.type = CT_PATLAK_CODE_BRANCH, .branches = window + 1});
    CTIndex branchStart = ct_patlak_codes_size(codes);
    for (CTIndex i = 0; i <= window; i++) {
        ct_patlak_codes_add(
            codes,
            (CTPatlakCode){.type = CT_PATLAK_CODE_EMPTY});
    }
    for (CTIndex i = 0; i < window; i++) {
        CTIndex start = ct_patlak_codes_size(codes);
        ct_patlak_compiler_object(codes, patterns, tree, index + 1);
        CTIndex branch                               = branchStart + i;
        ct_patlak_codes_get(codes, branch)->movement = start - branch;
    }
    CTIndex start                                = ct_patlak_codes_size(codes);
    CTIndex branch                               = branchStart + window;
    ct_patlak_codes_get(codes, branch)->movement = start - branch;
}

/* Compile the infinite repeat. */
void ct_patlak_compiler_repeat_infinite(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
    // Remove brackets.
    CTPatlakTokens inside = node->object.value;
    ct_patlak_tokens_unwrap(
        &inside,
        CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET,
        CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET);
    ct_expect(
        ct_patlak_tokens_size(&inside) > 0,
        "No tokens in the infinite repeat!");
    CTIndex lower = 0;
    switch (ct_patlak_tokens_first(&inside)) {
        case CT_PATLAK_TOKEN_NUMBER:
            ct_expect(
                ct_patlak_tokens_size(&inside) == 2,
                "Expected one more token after number in the infinite repeat!");
            ct_expect(
                ct_patlak_tokens_type(&inside, 1) == CT_PATLAK_TOKEN_COMMA,
                "Expected a comma after the numbers in the infinite repeat!");
            lower = (CTIndex)ct_string_parse(&inside.first[0].value);
            break;

        case CT_PATLAK_TOKEN_PLUS:
            ct_expect(
                ct_patlak_tokens_size(&inside) == 1,
                "Expected no more tokens after plus in the infinite repeat!");
            lower = 1;
            break;

        case CT_PATLAK_TOKEN_STAR:
            ct_expect(
                ct_patlak_tokens_size(&inside) == 1,
                "Expected no more tokens after star in the infinite repeat!");
            break;

        default:
            ct_unexpected("Unexpected token in the infinite repeat!");
    }
    for (CTIndex i = 0; i < lower; i++) {
        ct_patlak_compiler_object(codes, patterns, tree, index + 1);
    }
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.type = CT_PATLAK_CODE_BRANCH, .branches = 2});
    CTIndex branchStart = ct_patlak_codes_size(codes);
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.movement = 2, .type = CT_PATLAK_CODE_EMPTY});
    ct_patlak_codes_add(codes, (CTPatlakCode){.type = CT_PATLAK_CODE_EMPTY});

    CTIndex start = ct_patlak_codes_size(codes);
    ct_patlak_compiler_object(codes, patterns, tree, index + 1);

    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.type = CT_PATLAK_CODE_BRANCH, .branches = 2});
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){
            .movement = start - ct_patlak_codes_size(codes),
            .type     = CT_PATLAK_CODE_EMPTY});
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.movement = 1, .type = CT_PATLAK_CODE_EMPTY});

    start                                        = ct_patlak_codes_size(codes);
    CTIndex branch                               = branchStart + 1;
    ct_patlak_codes_get(codes, branch)->movement = start - branch;
}

/* Compile the function call. */
void ct_patlak_compiler_function_call(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index,
    CTPatlakNode const*     node)
{
}

/* Compile the object. */
void ct_patlak_compiler_object(
    CTPatlakCodes*          codes,
    CTPatlakPatterns const* patterns,
    CTPatlakTree const*     tree,
    CTIndex                 index)
{
    CTPatlakNode const* node = ct_patlak_tree_get(tree, index);
    switch (node->object.type) {
        case CT_PATLAK_OBJECT_LITERAL_REFERENCE:
            ct_patlak_compiler_reference(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_LITERAL_CHARACTER:
            ct_patlak_compiler_character(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_LITERAL_STRING:
            ct_patlak_compiler_string(codes, node);
            return;
        case CT_PATLAK_OBJECT_LITERAL_RANGE:
            ct_patlak_compiler_range(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_LITERAL_WILDCARD:
            ct_patlak_compiler_wildcard(codes);
            return;
        case CT_PATLAK_OBJECT_OR:
            ct_patlak_compiler_or(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_GROUP:
            ct_patlak_compiler_group(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_REPEAT_FIXED:
            ct_patlak_compiler_repeat_fixed(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_REPEAT_RANGE:
            ct_patlak_compiler_repeat_range(codes, patterns, tree, index, node);
            return;
        case CT_PATLAK_OBJECT_REPEAT_INFINITE:
            ct_patlak_compiler_repeat_infinite(
                codes,
                patterns,
                tree,
                index,
                node);
            return;
        case CT_PATLAK_OBJECT_FUNCTION_CALL:
            ct_patlak_compiler_function_call(
                codes,
                patterns,
                tree,
                index,
                node);
            return;
        default:
            break;
    }
    // DEBUG: Print unexpected object.
    printf("Unexpected Object: \"");
    ct_patlak_printer_object(&node->object);
    printf("\"\n");
    ct_unexpected("Unexpected object!");
}

/* Compile the syntax tree to the context. */
void ct_patlak_compiler(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree)
{
    // Store the start index to put into the hashmap.
    CTIndex start = ct_patlak_codes_size(codes);

    // Skip the equal sign, the name, and the parameters.
    ct_patlak_compiler_object(
        codes,
        patterns,
        tree,
        2 + ct_patlak_tree_get(tree, 1)->childeren);

    // Add the code that means a match to the end.
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.movement = 0, .type = CT_PATLAK_CODE_TERMINAL});

    // Put the pattern to the hashmap.
    ct_patlak_patterns_add(
        patterns,
        &tree->first[1].object.value.first->value,
        start);
}
