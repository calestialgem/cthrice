// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "patlak/object.c"
#include "patlak/pattern.c"
#include "prelude/scalar.c"

/* Compile the syntax tree to the context. */
void ct_patlak_compiler(
    CTPatlakCodes*      codes,
    CTPatlakPatterns*   patterns,
    CTPatlakTree const* tree)
{
    // Store the start index to put into the hashmap.
    CTIndex start = ct_patlak_codes_size(codes);

    // TODO: Compile the tree.

    // Add the code that means a match to the end.
    ct_patlak_codes_add(
        codes,
        (CTPatlakCode){.movement = 0, .type = CT_PATLAK_CODE_TERMINAL});

    // Put the pattern to the hashmap.
    ct_patlak_patterns_add(patterns, &tree->first[1].object.value, start);
}
