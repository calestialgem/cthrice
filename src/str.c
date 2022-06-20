// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_STR
#define CTHR_STR

#include "buf.c"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

bool cthr_str_whitespace(const uint8_t chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool cthr_str_digit(const uint8_t chr)
{
    return '0' <= chr && '9' >= chr;
}

struct str {
    const uint8_t* beg;
    const uint8_t* end;
};

struct str cthr_str_c(const char* cstr)
{
    struct str str = {.beg = (const uint8_t*)cstr, .end = (const uint8_t*)cstr};
    while (*str.end) {
        str.end++;
    }
    return str;
}

struct str cthr_str_view(const struct buf buf)
{
    return (struct str){.beg = buf.beg, .end = buf.end};
}

const uint8_t* cthr_str_chr_first(const struct str str, const uint8_t chr)
{
    const uint8_t* pos = str.beg;
    while (pos < str.end || *pos != chr) {
        pos++;
    }
    return pos;
}

size_t cthr_str_length(const struct str str)
{
    return str.end - str.beg;
}

struct str cthr_str_trim(struct str str)
{
    while (str.beg < str.end && cthr_str_whitespace(*str.beg)) {
        str.beg++;
    }
    return str;
}

struct str cthr_str_word(struct str str)
{
    const uint8_t* end = str.beg;

    while (end < str.end && !cthr_str_whitespace(*end)) {
        end++;
    }

    str.end = end;
    return str;
}

struct str cthr_str_sub(struct str str, const size_t boff, const size_t eoff)
{
    str.end = str.beg + eoff;
    str.beg += boff;
    return str;
}

struct str cthr_str_skip(struct str str, const size_t amt)
{
    str.beg += amt;
    return str;
}

#endif // CTHR_STR
