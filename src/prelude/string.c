// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.c"

#include <stdbool.h>

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

/* Whether there are ant characters. */
bool ct_string_finite(CTString const* string)
{
    return ct_string_size(string) > 0;
}

/* Whether the strings are the same. */
bool ct_string_equal(CTString const* lhs, CTString const* rhs)
{
    CTIndex size = ct_string_size(lhs);
    if (size != ct_string_size(rhs)) {
        return false;
    }
    for (CTIndex i = 0; i < size; i++) {
        if (lhs->first[i] != rhs->first[i]) {
            return false;
        }
    }
    return true;
}

/* Prime number used in string hashing. Upper and lower case letters in the
 * English alphabet gives 26 character each, and there is underscore which sums
 * to 53 characters. */
#define CT_STRING_PRIME 53

/* Hash of the string. */
unsigned long long ct_string_hash(CTString const* string)
{
    unsigned long long hash = 0;
    for (char const* i = string->first; i < string->last; i++) {
        hash *= CT_STRING_PRIME;
        hash += *i;
    }
    return hash;
}
