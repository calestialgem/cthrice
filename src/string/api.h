// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef STRING_H
#define STRING_H 1

#include "types/api.h"

/* Immutable array of bytes. */
struct str {
    /* Pointer to the first byte. */
    const byte* bgn;
    /* Pointer to the byte after the last one. */
    const byte* end;
};

/** Convert a static, null-terminated, C string to a string. */
struct str str_convert(const byte*);

/* Get the amount of bytes in the string. */
ptr str_size(struct str);
/* Check whether there are any bytes in the string. */
bool str_finite(struct str);
/* Check whether the strings are the equal. */
bool str_equal(struct str, struct str);

/** Find the first position of a byte that passes the predicate. Returns the
 * pointer to the byte after the last one if the byte does not exist. */
const byte* str_find_pred(struct str, bool (*)(byte));
/** Check whether a byte that passes the predicate exists in the string. */
bool str_contains_pred(struct str, bool (*)(byte));

/** Find the first position of the byte. Returns the pointer to the byte after
 * the last one if the byte does not exist. */
const byte* str_find(struct str, byte);
/** Check whether the byte exists in the string. */
bool str_contains(struct str, byte);

#endif // STRING_H
