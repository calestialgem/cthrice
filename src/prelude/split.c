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

/* Split at the border before the index. */
CTSplit ct_split(CTString const* string, CTIndex index)
{
    ct_expect(
        index >= 0 && index < ct_string_size(string),
        "Index out of string bounds!");
    return (CTSplit){
        .before = {        .first = string->first, .last = string->first + index},
        .after  = {.first = string->first + index,          .last = string->last}
    };
}
