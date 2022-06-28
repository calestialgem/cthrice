// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mod.h"

#include <stdio.h>

void cthrice_string_println(Cthrice_String str)
{
    printf("%.*s\n", (int)cthrice_string_length(str), str.bgn);
}

size_t cthrice_string_length(Cthrice_String str)
{
    return str.end - str.bgn;
}

bool cthrice_string_equals(Cthrice_String lhs, Cthrice_String rhs)
{
    size_t len = cthrice_string_length(lhs);
    if (len != cthrice_string_length(rhs)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (lhs.bgn[i] != rhs.bgn[i]) {
            return false;
        }
    }
    return true;
}

uint64_t cthrice_string_parse_u64(Cthrice_String str)
{
    uint64_t res = 0;
    for (const char* i = str.bgn; i < str.end; i++) {
        const uint64_t BASE = 10;
        res *= BASE;
        res += *i - '0';
    }
    return res;
}

Cthrice_String cthrice_string_static(const char* car)
{
    Cthrice_String str = {.bgn = car, .end = car};
    while (*str.end) {
        str.end++;
    }
    return str;
}

Cthrice_String cthrice_string_trim(Cthrice_String str)
{
    while (str.bgn < str.end && cthrice_whitespace(*str.bgn)) {
        str.bgn++;
    }
    return str;
}

Cthrice_String cthrice_string_part(Cthrice_String str, size_t bof, size_t eof)
{
    str.end = str.bgn + eof;
    str.bgn += bof;
    return str;
}

Cthrice_String cthrice_string_skip(Cthrice_String str, size_t amt)
{
    str.bgn += amt;
    return str;
}

const char* cthrice_string_first_pos_chr(Cthrice_String str, char chr)
{
    const char* pos = str.bgn;
    while (pos < str.end && *pos != chr) {
        pos++;
    }
    return pos;
}

Cthrice_String cthrice_string_first_word(Cthrice_String str)
{
    const char* end = str.bgn;
    while (end < str.end && !cthrice_whitespace(*end)) {
        end++;
    }
    str.end = end;
    return str;
}
