// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

/* Parts of a string splitted at a border. */
typedef struct {
    /* String before the border. */
    CTString before;
    /* String after the border. */
    CTString after;
} CTSplit;

/* Split at the border before the position. */
CTSplit ct_split_position(CTString const* string, char const* position)
{
    ct_expect(
        ct_string_valid_position(string, position),
        "Position out of string bounds!");
    return (CTSplit){
        .before = {.first = string->first,     .last = position},
        .after  = {     .first = position, .last = string->last}
    };
}

/* Split at the border before the index. */
CTSplit ct_split(CTString const* string, CTIndex index)
{
    return ct_split_position(string, string->first + index);
}

/* Split at the first character that fits the predicate. */
CTSplit ct_split_first_fit(CTString const* string, bool (*predicate)(char))
{
    return ct_split_position(string, ct_string_first_fit(string, predicate));
}

/* Split at the first occurance of the character. */
CTSplit ct_split_first(CTString const* string, char character)
{
    return ct_split_position(string, ct_string_first(string, character));
}
