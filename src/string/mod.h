// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING_H
#define CTHRICE_STRING_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Check if the character is whitespace: one of carrige return, new line, tab,
 * and space. */
bool cthrice_whitespace(char chr);
/** Check if the character is part of English alphabet. */
bool cthrice_letter(char chr);

/** Check if the character is a decimal digit. */
bool cthrice_digit(char chr);
/** Check if the character is a binary digit. */
bool cthrice_digit_bin(char chr);
/** Check if the character is a octal digit. */
bool cthrice_digit_oct(char chr);
/** Check if the character is a hexadecimal digit. */
bool cthrice_digit_hex(char chr);

/** Nonowning string: holds a pointer to a chunk of data. */
typedef struct {
    const char* bgn;
    const char* end;
} Cthrice_String;

/** Print the string. */
void cthrice_string_println(Cthrice_String str);

/** Get the amount of bytes in the string. */
size_t cthrice_string_length(Cthrice_String str);
/** Check if the data pointed by the strings are the same. */
bool cthrice_string_equals(Cthrice_String lhs, Cthrice_String rhs);
/** Parse the string to an unsigned integer. */
uint64_t cthrice_string_parse_u64(Cthrice_String str);

/** Convert a static, null-terminated, C string. */
Cthrice_String cthrice_string_static(const char* car);
/** Create a new string without the leading whitespace. */
Cthrice_String cthrice_string_trim(Cthrice_String str);
/** Create a new string whose begining and end location are defined as byte
 * offsets from the start of the string. */
Cthrice_String cthrice_string_part(Cthrice_String str, size_t bof, size_t eof);
/** Create a new string without the given amount of bytes at the start. */
Cthrice_String cthrice_string_skip(Cthrice_String str, size_t amt);

/** Return the pointer to the given byte in the string. Returns the end of the
 * string if the byte does not exist. */
const char* cthrice_string_first_pos_chr(Cthrice_String str, char chr);
/** Create a new string from the start up to the first whitespace character. If
 * the string does not have a whitespace, the result is an equal string. */
Cthrice_String cthrice_string_first_word(Cthrice_String str);

#endif // CTHRICE_STRING_H
