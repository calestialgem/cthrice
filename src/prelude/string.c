// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.c"
#include "prelude/scalar.c"

#include <math.h>
#include <stdbool.h>

/* Immutable pointers to contiguous characters. */
typedef struct {
    /* Border before the first character. */
    char const* first;
    /* Border after the last character. */
    char const* last;
} CTString;

/* Convert a null terminated string. */
CTString ct_string_terminated(char const* terminated_string)
{
    CTString result = {.first = terminated_string, .last = terminated_string};
    while (*result.last) {
        result.last++;
    }
    return result;
}

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

/* Whether the position is valid. */
bool ct_string_valid_position(CTString const* string, char const* position)
{
    return position >= string->first && position <= string->last;
}

/* Character at the index. */
char ct_string_at(CTString const* string, CTIndex index)
{
    char const* position = string->first + index;
    ct_expect(
        ct_string_valid_position(string, position),
        "Index out of string bounds!");
    return *position;
}

/* Find the first character that fits the predicate. Returns the position after
 * the last character if none fits it. */
char const* ct_string_first_fit(CTString const* string, bool (*predicate)(char))
{
    for (char const* i = string->first; i < string->last; i++) {
        if (predicate(*i)) {
            return i;
        }
    }
    return string->last;
}

/* Find the first occurance of the character. Returns the position after
 * the last character if it does not exist. */
char const* ct_string_first(CTString const* string, char character)
{
    for (char const* i = string->first; i < string->last; i++) {
        if (*i == character) {
            return i;
        }
    }
    return string->last;
}

/* Whether the string starts with the character. */
bool ct_string_starts(CTString const* string, char character)
{
    ct_expect(ct_string_finite(string), "There are no characters!");
    return *string->first == character;
}

/* Whether the string finishes with the character. */
bool ct_string_finishes(CTString const* string, char character)
{
    ct_expect(ct_string_finite(string), "There are no characters!");
    return *(string->last - 1) == character;
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

/* Decimal number in the string. */
unsigned long long ct_string_parse(CTString const* string)
{
    unsigned long long const BASE   = 10;
    unsigned long long       result = 0;
    for (char const* i = string->first; i < string->last; i++) {
        result *= BASE;
        result += (*i - '0');
    }
    return result;
}
