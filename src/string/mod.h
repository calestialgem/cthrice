// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING_H
#define CTHRICE_STRING_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool cthrice_whitespace(char chr);
bool cthrice_letter(char chr);

bool cthrice_digit(char chr);
bool cthrice_digit_bin(char chr);
bool cthrice_digit_oct(char chr);
bool cthrice_digit_hex(char chr);

typedef struct {
    const char* bgn;
    const char* end;
} Cthrice_String;

void cthrice_string_println(Cthrice_String str);

size_t   cthrice_string_length(Cthrice_String str);
bool     cthrice_string_equals(Cthrice_String lhs, Cthrice_String rhs);
uint64_t cthrice_string_parse_u64(Cthrice_String str);

Cthrice_String cthrice_string_static(const char* car);
Cthrice_String cthrice_string_trim(Cthrice_String str);
Cthrice_String cthrice_string_part(Cthrice_String str, size_t bof, size_t eof);
Cthrice_String cthrice_string_skip(Cthrice_String str, size_t amt);

const char*    cthrice_string_first_pos_chr(Cthrice_String str, char chr);
Cthrice_String cthrice_string_first_word(Cthrice_String str);

#endif // CTHRICE_STRING_H
