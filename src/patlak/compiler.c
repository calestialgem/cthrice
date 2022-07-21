// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "patlak/object.c"
#include "patlak/pattern.c"
#include "patlak/printer.c"
#include "prelude/expect.c"
#include "prelude/scalar.c"

#include <stdio.h>

// For use by unit compilers.
void ct_patlak_compiler_object(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index);

/* Compile the reference literal at the index. */
void ct_patlak_compiler_reference(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the character literal at the index. */
void ct_patlak_compiler_character(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the string literal at the index. */
void ct_patlak_compiler_string(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the character range literal at the index. */
void ct_patlak_compiler_range(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the wildcard literal at the index. */
void ct_patlak_compiler_wildcard(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the OR binary operator at the index. */
void ct_patlak_compiler_or(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the group at the index. */
void ct_patlak_compiler_group(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the fixed repeat at the index. */
void ct_patlak_compiler_repeat_fixed(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the ranged repeat at the index. */
void ct_patlak_compiler_repeat_range(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the infinite repeat at the index. */
void ct_patlak_compiler_repeat_infinite(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the function call at the index. */
void ct_patlak_compiler_function_call(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
}

/* Compile the object at the index. */
void ct_patlak_compiler_object(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree,
    CTIndex             index)
{
    CTPatlakNode* node = ct_patlak_tree_get(tree, index);
    switch (node->object.type) {
        case CT_PATLAK_OBJECT_LITERAL_REFERENCE:
            ct_patlak_compiler_reference(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_LITERAL_CHARACTER:
            ct_patlak_compiler_character(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_LITERAL_STRING:
            ct_patlak_compiler_string(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_LITERAL_RANGE:
            ct_patlak_compiler_range(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_LITERAL_WILDCARD:
            ct_patlak_compiler_wildcard(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_OR:
            ct_patlak_compiler_or(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_GROUP:
            ct_patlak_compiler_group(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_REPEAT_FIXED:
            ct_patlak_compiler_repeat_fixed(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_REPEAT_RANGE:
            ct_patlak_compiler_repeat_range(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_REPEAT_INFINITE:
            ct_patlak_compiler_repeat_infinite(codes, patterns, tree, index);
            return;
        case CT_PATLAK_OBJECT_FUNCTION_CALL:
            ct_patlak_compiler_function_call(codes, patterns, tree, index);
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
    ct_patlak_patterns_add(patterns, &tree->first[1].object.value, start);
}
