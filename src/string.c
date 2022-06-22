// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_STRING
#define THRICE_STRING

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool thriceWhitespace(const uint8_t chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool thriceLetter(const uint8_t chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool thriceDigit(const uint8_t chr)
{
    return '0' <= chr && '9' >= chr;
}

bool thriceDigitBin(const uint8_t chr)
{
    return '0' <= chr && '1' >= chr;
}

bool thriceDigitOct(const uint8_t chr)
{
    return '0' <= chr && '7' >= chr;
}

bool thriceDigitHex(const uint8_t chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}

typedef struct {
    const uint8_t* beg;
    const uint8_t* end;
} thr_str;

thr_str thriceStringStatic(const char* cstr)
{
    thr_str str = {.beg = (const uint8_t*)cstr, .end = (const uint8_t*)cstr};
    while (*str.end) {
        str.end++;
    }
    return str;
}

const uint8_t* thriceStringFirstPosChr(const thr_str str, const uint8_t chr)
{
    const uint8_t* pos = str.beg;
    while (pos < str.end && *pos != chr) {
        pos++;
    }
    return pos;
}

size_t thriceStringLength(const thr_str str)
{
    return str.end - str.beg;
}

bool thriceStringEquals(const thr_str lhs, const thr_str rhs)
{
    size_t len = thriceStringLength(lhs);
    if (len != thriceStringLength(rhs)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        if (*(lhs.beg + i) != *(rhs.beg + i)) {
            return false;
        }
    }
    return true;
}

thr_str thriceStringTrim(thr_str str)
{
    while (str.beg < str.end && thriceWhitespace(*str.beg)) {
        str.beg++;
    }
    return str;
}

thr_str thriceStringFirstWord(thr_str str)
{
    const uint8_t* end = str.beg;

    while (end < str.end && !thriceWhitespace(*end)) {
        end++;
    }

    str.end = end;
    return str;
}

thr_str thriceStringPart(thr_str str, const size_t boff, const size_t eoff)
{
    str.end = str.beg + eoff;
    str.beg += boff;
    return str;
}

thr_str thriceStringSkip(thr_str str, const size_t amt)
{
    str.beg += amt;
    return str;
}

uint64_t thriceStringParseU64(thr_str str)
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