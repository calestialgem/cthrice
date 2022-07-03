// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "string/api.h"

#include <stdio.h>
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

struct str bfr_view_sub(struct bfr bfr, ptr off)
{
    return (struct str){.bgn = bfr.bgn + off, .end = bfr.end};
}

struct str bfr_view(struct bfr bfr)
{
    return bfr_view_sub(bfr, 0);
}

struct bfr bfr_destroy(struct bfr bfr)
{
    free(bfr.bgn);
    bfr.bgn = bfr.end = bfr.lst = null;
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
    ptr grw = max(ned, cap / 2);
    ptr nwc = cap + grw;

    ASSERT(nwc > 0, "New buffer capacity is not positive!");

    byte* mem = realloc(bfr.bgn, nwc);
    CHECK(mem != null, "Could not allocate buffer!");

    // Recalculate pointers.
    bfr.bgn = mem;
    bfr.end = mem + sze;
    bfr.lst = mem + nwc;
    return bfr;
}

struct bfr bfr_put(struct bfr bfr, byte chr)
{
    bfr        = bfr_ensure_space(bfr, 1);
    *bfr.end++ = chr;
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

struct bfr bfr_put_file(struct bfr bfr, const byte* path)
{
    FILE* file = 0;
    CHECK(fopen_s(&file, path, "r") == 0, "Could not open the file!");

    const ptr CHUNK = 1024;
    ptr       wrt   = 0;

    do {
        bfr = bfr_ensure_space(bfr, CHUNK);
        wrt = (ptr)fread(bfr.end, 1, CHUNK, file);
        bfr.end += wrt;
    } while (CHUNK == wrt);

    CHECK(feof(file), "Problem while reading!");
    CHECK(fclose(file) != -1, "Could not close the file!");

    return bfr;
}
