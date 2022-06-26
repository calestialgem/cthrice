// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING
#define CTHRICE_STRING

#include "scalar.c"

#include <stdio.h>

bool cthrice_whitespace(uchr chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool cthrice_letter(uchr chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool cthrice_digit(uchr chr)
{
    return '0' <= chr && '9' >= chr;
}

bool cthrice_digit_bin(uchr chr)
{
    return '0' <= chr && '1' >= chr;
}

bool cthrice_digit_oct(uchr chr)
{
    return '0' <= chr && '7' >= chr;
}

bool cthrice_digit_hex(uchr chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}

typedef struct {
    uchr* bgn;
    uchr* end;
} Cthrice_String;

Cthrice_String cthrice_string_static(ichr* cstr)
{
    Cthrice_String str = {.bgn = (uchr*)cstr, .end = (uchr*)cstr};
    while (*str.end) {
        str.end++;
    }
    return str;
}

uchr* cthrice_string_first_pos_chr(Cthrice_String str, uchr chr)
{
    uchr* pos = str.bgn;
    while (pos < str.end && *pos != chr) {
        pos++;
    }
    return pos;
}

uptr cthrice_string_length(Cthrice_String str)
{
    return str.end - str.bgn;
}

bool cthrice_string_equals(Cthrice_String lhs, Cthrice_String rhs)
{
    uptr len = cthrice_string_length(lhs);
    if (len != cthrice_string_length(rhs)) {
        return false;
    }
    for (uptr i = 0; i < len; i++) {
        if (lhs.bgn[i] != rhs.bgn[i]) {
            return false;
        }
    }
    return true;
}

void cthrice_string_println(Cthrice_String str)
{
    printf("%*.s\n", (int)cthrice_string_length(str), str.bgn);
}

Cthrice_String cthrice_string_trim(Cthrice_String str)
{
    while (str.bgn < str.end && cthrice_whitespace(*str.bgn)) {
        str.bgn++;
    }
    return str;
}

Cthrice_String cthrice_string_first_word(Cthrice_String str)
{
    uchr* end = str.bgn;

    while (end < str.end && !cthrice_whitespace(*end)) {
        end++;
    }

    str.end = end;
    return str;
}

Cthrice_String cthrice_string_part(Cthrice_String str, uptr boff, uptr eoff)
{
    str.end = str.bgn + eoff;
    str.bgn += boff;
    return str;
}

Cthrice_String cthrice_string_skip(Cthrice_String str, uptr amt)
{
    str.bgn += amt;
    return str;
}

unt64 cthrice_string_parse_u64(Cthrice_String str)
{
    unt64 res = 0;

    for (uchr* i = str.bgn; i < str.end; i++) {
        const unt64 BASE = 10;
        res *= BASE;
        res += *i - '0';
    }

    return res;
}

#endif // CTHRICE_STRING
