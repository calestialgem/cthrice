// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_buffer.h"

#include "cthrice_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Cthrice_Buffer cthrice_buffer_create(uptr cap)
{
    unt8* restrict mem = malloc(cap);
    if (!mem) {
        cthrice_error_alloc();
    }
    return (Cthrice_Buffer){.bgn = mem, .end = mem, .lst = mem + cap};
}

Cthrice_Buffer cthrice_buffer_destroy(Cthrice_Buffer bfr)
{
    free(bfr.bgn);
    return (Cthrice_Buffer){.bgn = 0, .end = 0, .lst = 0};
}

uptr cthrice_buffer_size(Cthrice_Buffer bfr)
{
    return bfr.end - bfr.bgn;
}

uptr cthrice_buffer_capacity(Cthrice_Buffer bfr)
{
    return bfr.lst - bfr.bgn;
}

uptr cthrice_buffer_space(Cthrice_Buffer bfr)
{
    return bfr.lst - bfr.end;
}

Cthrice_String cthrice_buffer_view(Cthrice_Buffer bfr)
{
    return (Cthrice_String){.bgn = bfr.bgn, .end = bfr.end};
}

Cthrice_Buffer cthrice_buffer_clear(Cthrice_Buffer bfr)
{
    bfr.end = bfr.bgn;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_recap(Cthrice_Buffer bfr, uptr cap)
{
    if (!cap) {
        return cthrice_buffer_destroy(bfr);
    }
    if (!bfr.bgn) {
        return cthrice_buffer_create(cap);
    }
    unt8* restrict mem = realloc(bfr.bgn, cap);
    if (!mem) {
        cthrice_error_alloc();
    }
    bfr.lst = mem + cap;
    bfr.end = mem + min(cap, cthrice_buffer_size(bfr));
    bfr.bgn = mem;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_unt8(Cthrice_Buffer bfr, unt8 chr)
{
    if (cthrice_buffer_space(bfr) < 1) {
        uptr cap = cthrice_buffer_capacity(bfr);
        bfr      = cthrice_buffer_recap(bfr, max(1, 2 * cap));
    }

    *(bfr.end) = chr;
    bfr.end++;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_unt64(Cthrice_Buffer bfr, unt64 unt)
{
    const unt64 BASE  = 10;
    unt64       place = BASE;

    while (1 != place) {
        if (unt > place) {
            place *= BASE;
            break;
        }
        place /= BASE;
        bfr = cthrice_buffer_append_unt8(bfr, '0' + (unt8)(unt / place));
        unt %= place;
    }

    return bfr;
}

Cthrice_Buffer
cthrice_buffer_append_string(Cthrice_Buffer bfr, Cthrice_String str)
{
    uptr spc = cthrice_buffer_space(bfr);
    uptr len = cthrice_string_length(str);
    if (spc < len) {
        uptr cap = cthrice_buffer_capacity(bfr);
        bfr      = cthrice_buffer_recap(bfr, cap + len - spc);
    }

    memcpy(bfr.end, str.bgn, len);
    bfr.end += len;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_file(Cthrice_Buffer bfr, ichr* name)
{
    FILE* file = 0;
    if (fopen_s(&file, name, "r")) {
        cthrice_error("Could not open the file!");
    }

    const uptr CHUNK = 1024;
    uptr       wrt   = 0;

    do {
        uptr spc = cthrice_buffer_space(bfr);
        if (spc < CHUNK) {
            bfr = cthrice_buffer_recap(bfr, CHUNK - spc);
        }
        wrt = fread(bfr.end, 1, CHUNK, file);
        bfr.end += wrt;
    } while (CHUNK == wrt);

    if (!feof(file)) {
        cthrice_error("Problem while reading!");
    }

    if (fclose(file) == -1) {
        cthrice_error("Could not close the file!");
    }

    return bfr;
}
