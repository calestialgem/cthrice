// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_STRING
#define THRICE_STRING

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool ThriceWhitespace(const uint8_t chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool ThriceLetter(const uint8_t chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool ThriceDigit(const uint8_t chr)
{
    return '0' <= chr && '9' >= chr;
}

bool ThriceDigitBin(const uint8_t chr)
{
    return '0' <= chr && '1' >= chr;
}

bool ThriceDigitOct(const uint8_t chr)
{
    return '0' <= chr && '7' >= chr;
}

bool ThriceDigitHex(const uint8_t chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}

typedef struct {
    const uint8_t* beg;
    const uint8_t* end;
} ThriceString;

ThriceString ThriceStringStatic(const char* cstr)
{
    ThriceString str = {
        .beg = (const uint8_t*)cstr,
        .end = (const uint8_t*)cstr};
    while (*str.end) {
        str.end++;
    }
    return str;
}

const uint8_t*
ThriceStringFirstPosChr(const ThriceString str, const uint8_t chr)
{
    const uint8_t* pos = str.beg;
    while (pos < str.end && *pos != chr) {
        pos++;
    }
    return pos;
}

size_t ThriceStringLength(const ThriceString str)
{
    return str.end - str.beg;
}

bool ThriceStringEquals(const ThriceString lhs, const ThriceString rhs)
{
    size_t len = ThriceStringLength(lhs);
    if (len != ThriceStringLength(rhs)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (*(lhs.beg + i) != *(rhs.beg + i)) {
            return false;
        }
    }
    return true;
}

ThriceString ThriceStringTrim(ThriceString str)
{
    while (str.beg < str.end && ThriceWhitespace(*str.beg)) {
        str.beg++;
    }
    return str;
}

ThriceString ThriceStringFirstWord(ThriceString str)
{
    const uint8_t* end = str.beg;

    while (end < str.end && !ThriceWhitespace(*end)) {
        end++;
    }

    str.end = end;
    return str;
}

ThriceString
ThriceStringPart(ThriceString str, const size_t boff, const size_t eoff)
{
    str.end = str.beg + eoff;
    str.beg += boff;
    return str;
}

ThriceString ThriceStringSkip(ThriceString str, const size_t amt)
{
    str.beg += amt;
    return str;
}

uint64_t ThriceStringParseU64(ThriceString str)
{
    uint64_t res = 0;

    for (const uint8_t* i = str.beg; i < str.end; i++) {
        const uint64_t base = 10;
        res *= base;
        res += *i - '0';
    }

    return res;
}

#endif // THRICE_STRING
