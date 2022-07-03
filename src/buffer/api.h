// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BUFFER_H
#define BUFFER_H 1

#include "types/api.h"

/* Dynamic array of bytes. */
struct bfr {
    /* Begging of the allocated bytes in the buffer. */
    byte* bgn;
    /* The byte after the last valid byte in the buffer. */
    byte* end;
    /* The byte after the allocated bytes in the buffer. */
    byte* lst;
};

/* Get the amount of valid bytes in the buffer. */
ptr bfr_size(struct bfr);
/* Get the amount of allocated bytes in the buffer. */
ptr bfr_capacity(struct bfr);
/* Get the amount of unused bytes in the buffer. */
ptr bfr_space(struct bfr);

/* Deallocate the bytes in the buffer. */
[[nodiscard]] struct bfr bfr_destroy(struct bfr);
/* Ensure that the buffer has at least the given amount of space. If it is
 * necessary, grows the buffer at least by the half of the capacity. */
[[nodiscard]] struct bfr bfr_ensure_space(struct bfr, ptr);
/* Put a byte at the end of the buffer. Grows the buffer if necessary. */
[[nodiscard]] struct bfr bfr_put(struct bfr, byte);

#endif // BUFFER_H
