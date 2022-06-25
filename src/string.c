// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING
#define CTHRICE_STRING

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool cthrice_whitespace(uint8_t chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool cthrice_letter(uint8_t chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool cthrice_digit(uint8_t chr)
{
    return '0' <= chr && '9' >= chr;
}

bool cthrice_digit_bin(uint8_t chr)
{
    return '0' <= chr && '1' >= chr;
}

bool cthrice_digit_oct(uint8_t chr)
{
    return '0' <= chr && '7' >= chr;
}

bool cthrice_digit_hex(uint8_t chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}

typedef struct {
    const uint8_t* bgn;
    const uint8_t* end;
} Cthrice_String;

Cthrice_String cthrice_string_static(const char* cstr)
{
    Cthrice_String str = {
        .bgn = (const uint8_t*)cstr,
        .end = (const uint8_t*)cstr};
    while (*str.end) {
        str.end++;
    }
    return str;
}

const uint8_t* cthrice_string_first_pos_chr(Cthrice_String str, uint8_t chr)
{
    const uint8_t* pos = str.bgn;
    while (pos < str.end && *pos != chr) {
        pos++;
    }
    return pos;
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
        if (*(lhs.bgn + i) != *(rhs.bgn + i)) {
            return false;
        }
    }
    return true;
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
    const uint8_t* end = str.bgn;

    while (end < str.end && !cthrice_whitespace(*end)) {
        end++;
    }

    str.end = end;
    return str;
}

Cthrice_String cthrice_string_part(Cthrice_String str, size_t boff, size_t eoff)
{
    str.end = str.bgn + eoff;
    str.bgn += boff;
    return str;
}

Cthrice_String cthrice_string_skip(Cthrice_String str, size_t amt)
{
    str.bgn += amt;
    return str;
}

uint64_t cthrice_string_parse_u64(Cthrice_String str)
{
    uint64_t res = 0;

    for (const uint8_t* i = str.bgn; i < str.end; i++) {
        const uint64_t BASE = 10;
        res *= BASE;
        res += *i - '0';
    }

    return res;
}

#endif // CTHRICE_STRING
