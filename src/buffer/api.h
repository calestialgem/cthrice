// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BUFFER_H
#define BUFFER_H 1

#include <stddef.h>

/* Dynamic array of characters. */
typedef struct {
    /* Begging of the allocated bytes in the buffer. */
    char* bgn;
    /* The byte after the last valid byte in the buffer. */
    char* end;
    /* The byte after the allocated bytes in the buffer. */
    char* lst;
} cthrbfr;

/* Get the amount of valid bytes in the buffer. */
ptrdiff_t bfr_size(cthrbfr);
/* Get the amount of allocated bytes in the buffer. */
ptrdiff_t bfr_capacity(cthrbfr);
/* Get the amount of unused bytes in the buffer. */
ptrdiff_t bfr_space(cthrbfr);

/* Deallocate the bytes in the buffer. */
[[nodiscard]] cthrbfr bfr_destroy(cthrbfr);
/* Ensure that the buffer has at least the given amount of space. If it is
 * necessary, grows the buffer at least by the half of the capacity. */
[[nodiscard]] cthrbfr bfr_ensure_space(cthrbfr, ptrdiff_t);
/* Put a byte at the end of the buffer. Grows the buffer if necessary. */
[[nodiscard]] cthrbfr bfr_put(cthrbfr, char);

#endif // BUFFER_H
