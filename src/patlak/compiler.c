// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/context.c"
#include "patlak/object.c"
#include "prelude/scalar.c"

/* Compile the syntax tree to the context. */
void ct_patlak_compiler(CTPatlakContext* context, CTPatlakTree const* tree)
{
    // Store the start index to put into the hashmap.
    CTIndex start = ct_patlak_codes_size(&context->codes);

    // TODO: Compile the tree.

    // Add the code that means a match to the end.
    ct_patlak_codes_add(
        &context->codes,
        (CTPatlakCode){.movement = 0, .type = CT_PATLAK_CODE_TERMINAL});

    // Put the pattern to the hashmap.
    ct_patlak_patterns_add(
        &context->patterns,
        &tree->first[1].object.value,
        start);
}
