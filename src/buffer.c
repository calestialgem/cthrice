// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_BUFFER
#define CTHRICE_BUFFER

#include "error.c"
#include "scalar.c"
#include "string.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unt8* restrict bgn;
    unt8* restrict end;
    unt8* restrict lst;
} Cthrice_Buffer;

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

Cthrice_Buffer cthrice_buffer_grow(Cthrice_Buffer bfr, uptr cap)
{
    if (!cap) {
        cap = 1;
    }
    unt8* restrict mem = realloc(bfr.bgn, cap);
    if (!mem) {
        cthrice_error_alloc();
    }
    if (!bfr.bgn && mem != bfr.bgn) {
        memcpy(mem, bfr.bgn, cthrice_buffer_size(bfr));
        free(bfr.bgn);
        bfr.bgn = mem;
    }
    bfr.lst = mem + cap;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_unt8(Cthrice_Buffer bfr, unt8 chr)
{
    if (cthrice_buffer_space(bfr) < 1) {
        bfr = cthrice_buffer_grow(bfr, 2 * cthrice_buffer_capacity(bfr));
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
        bfr = cthrice_buffer_grow(bfr, len - spc);
    }

    memcpy(bfr.end, str.bgn, len);
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_file(Cthrice_Buffer bfr, ichr* name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        cthrice_error("Could not open file!");
    }

    const uptr CHUNK = 1024;
    uptr       wrt   = 0;

    do {
        uptr spc = cthrice_buffer_space(bfr);
        if (spc < CHUNK) {
            bfr = cthrice_buffer_grow(bfr, CHUNK - spc);
        }
        wrt = fread(bfr.end, 1, CHUNK, file);
        bfr.end += wrt;
    } while (CHUNK == wrt);

    if (!feof(file)) {
        cthrice_error("Problem while reading!");
    }

    fclose(file);
    return bfr;
}

#endif // CTHRICE_BUFFER
