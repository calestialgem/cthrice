// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "string/api.h"

#include <stdlib.h>
#include <string.h>

ptr bfr_size(struct bfr bfr)
{
    ASSERT(bfr.end >= bfr.bgn, "Buffer does not have a positive size!");
    return bfr.end - bfr.bgn;
}

ptr bfr_capacity(struct bfr bfr)
{
    ASSERT(bfr.lst >= bfr.bgn, "Buffer does not have a positive capacity!");
    return bfr.lst - bfr.bgn;
}

ptr bfr_space(struct bfr bfr)
{
    ASSERT(bfr.lst >= bfr.end, "Buffer does not have a positive space!");
    return bfr.lst - bfr.end;
}

struct bfr bfr_destroy(struct bfr bfr)
{
    free(bfr.bgn);
    bfr.bgn = bfr.end = bfr.lst = 0;
    return bfr;
}

struct bfr bfr_ensure_space(struct bfr bfr, ptr spc)
{
    ASSERT(spc >= 0, "Buffer space requested is negative!");

    // If the current space is enough, return the same buffer.
    ptr curspc = bfr_space(bfr);
    if (curspc >= spc) {
        return bfr;
    }

    // Store the size to recalculate the end pointer.
    ptr sze = bfr_size(bfr);

    // Find the new capacity.
    ptr ned = spc - curspc;
    ptr cap = bfr_capacity(bfr);
    ptr grw = min(ned, cap / 2);
    ptr nwc = cap + grw;

    ASSERT(nwc > 0, "New buffer capacity is not positive!");

    byte* mem = realloc(bfr.bgn, nwc);
    CHECK(mem != 0, "Could not allocate buffer!");

    // Recalculate pointers.
    bfr.bgn = mem;
    bfr.end = mem + sze;
    bfr.lst = mem + nwc;
    return bfr;
}

struct bfr bfr_put(struct bfr bfr, byte chr)
{
    bfr          = bfr_ensure_space(bfr, 1);
    *(bfr.end++) = chr;
    return bfr;
}

struct bfr bfr_put_str(struct bfr bfr, struct str str)
{
    ptr sze = str_size(str);
    bfr     = bfr_ensure_space(bfr, sze);
    memcpy(bfr.end, str.bgn, sze);
    bfr.end += sze;
    return bfr;
}
