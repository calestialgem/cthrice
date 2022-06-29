// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_BUFFER_H
#define CTHRICE_BUFFER_H 1

#include "string/mod.h"

#include <stdint.h>

/** Create the buffer. */
void cthrice_buffer_create();
/** Destroy the buffer. */
void cthrice_buffer_destroy();

/** Get the current end of the buffer. This should be used for keeping track of
 * where you started to write to the buffer. */
const char* cthrice_buffer_pos();
/** Align the buffer pos by skipping. This is useful to store data. Returns the
 * new end of the buffer, which is aligned with the given alignement. */
const char* cthrice_buffer_skip(size_t ali);

/** Append a character to the buffer. */
void cthrice_buffer_append_chr(char chr);
/** Append a string to the buffer. */
void cthrice_buffer_append_string(Cthrice_String str);

/** Append an unsigned integer to the buffer. Returns the resulting string. */
Cthrice_String cthrice_buffer_append_unt(uint64_t unt);
/** Append a file to the buffer. Returns the resulting string. */
Cthrice_String cthrice_buffer_append_file(const char* pth);

#endif // CTHRICE_BUFFER_H
