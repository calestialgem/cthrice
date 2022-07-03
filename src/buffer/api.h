// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BUFFER_H
#define BUFFER_H 1

#include "string/api.h"
#include "types/api.h"

/* Dynamic array of bytes. */
struct bfr {
    /* Pointer to the first allocated byte. */
    byte* bgn;
    /* Pointer to the byte after the last valid one. */
    byte* end;
    /* Pointer to the byte after the last allocated one. */
    byte* lst;
};

/* Get the amount of valid bytes in the buffer. */
ptr bfr_size(struct bfr);
/* Get the amount of allocated bytes in the buffer. */
ptr bfr_capacity(struct bfr);
/* Get the amount of unused bytes in the buffer. */
ptr bfr_space(struct bfr);

/* Deallocate the bytes in the buffer. */
struct bfr bfr_destroy(struct bfr);
/* Ensure that the buffer has at least the given amount of space. If it is
 * necessary, grows the buffer at least by the half of the capacity. */
struct bfr bfr_ensure_space(struct bfr, ptr);
/* Put a byte at the end of the buffer. Grows the buffer if necessary. */
struct bfr bfr_put(struct bfr, byte);
/* Put a string at the end of the buffer. Grows the buffer if necessary. */
struct bfr bfr_put_str(struct bfr, struct str);

#endif // BUFFER_H
