// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "prelude/string.c"

/* All pattern related data. */
typedef struct {
    /* Compiled pattern code. */
    CTPatlakCodes codes;
    /* Pattern informations. */
    CTPatlakPatterns patterns;
} CTPatlakContext;

/* Compile the pattern by searching for references in the context. */
void ct_patlak_compile(CTPatlakContext* context, CTString const* pattern) {}

/* Match the pattern with the name to the input. Returns the initial portion of
 * the input that matched. Matches are checked from the begining. Empty match
 * means it did not match or the pattern was not found. */
CTString ct_patlak_match(
    CTPatlakContext const* context,
    CTString const*        name,
    CTString const*        input)
{
}

/* Deallocate the memory. */
void ct_patlak_free(CTPatlakContext* context)
{
    ct_patlak_codes_free(&context->codes);
}
