// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
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

/** Append a character to the buffer. */
void cthrice_buffer_append_chr(char chr);
/** Append an unsigned integer to the buffer. */
void cthrice_buffer_append_unt(uint64_t unt);
/** Append a string to the buffer. */
void cthrice_buffer_append_string(Cthrice_String str);
/** Append a file to the buffer. */
void cthrice_buffer_append_file(const char* pth);

#endif // CTHRICE_BUFFER_H
