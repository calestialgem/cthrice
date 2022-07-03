// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

ptrdiff_t bfr_size(cthrbfr bfr)
{
    ASSERT(bfr.end >= bfr.bgn, "Buffer does not have a positive size!");
    return bfr.end - bfr.bgn;
}

ptrdiff_t bfr_capacity(cthrbfr bfr)
{
    ASSERT(bfr.lst >= bfr.bgn, "Buffer does not have a positive capacity!");
    return bfr.lst - bfr.bgn;
}

ptrdiff_t bfr_space(cthrbfr bfr)
{
    ASSERT(bfr.lst >= bfr.end, "Buffer does not have a positive space!");
    return bfr.lst - bfr.end;
}

[[nodiscard]] cthrbfr bfr_destroy(cthrbfr bfr)
{
    free(bfr.bgn);
    bfr.bgn = bfr.end = bfr.lst = 0;
    return bfr;
}

[[nodiscard]] cthrbfr bfr_ensure_space(cthrbfr bfr, ptrdiff_t spc)
{
    ASSERT(spc >= 0, "Buffer space requested is negative!");

    // If the current space is enough, return the same buffer.
    ptrdiff_t curspc = bfr_space(bfr);
    if (curspc >= spc) {
        return bfr;
    }

    // Store the size to recalculate the end pointer.
    ptrdiff_t sze = bfr_size(bfr);

    // Find the new capacity.
    ptrdiff_t ned = spc - curspc;
    ptrdiff_t cap = bfr_capacity(bfr);
    ptrdiff_t grw = min(ned, cap / 2);
    ptrdiff_t nwc = cap + grw;

    ASSERT(nwc > 0, "New buffer capacity is not positive!");

    char* mem = realloc(bfr.bgn, nwc);
    CHECK(mem != 0, "Could not allocate buffer!");

    // Recalculate pointers.
    bfr.bgn = mem;
    bfr.end = mem + sze;
    bfr.lst = mem + nwc;
    return bfr;
}

[[nodiscard]] cthrbfr bfr_put(cthrbfr bfr, char chr)
{
    bfr          = bfr_ensure_space(bfr, 1);
    *(bfr.end++) = chr;
    return bfr;
}
