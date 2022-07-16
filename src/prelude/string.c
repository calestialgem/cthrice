// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.c"

/* Immutable pointers to contiguous characters. */
typedef struct {
    /* Border before the first character. */
    char const* first;
    /* Border after the last character. */
    char const* last;
} CTString;

/* Amount of characters. */
CTIndex ct_string_size(CTString const* string)
{
    return string->last - string->first;
}
