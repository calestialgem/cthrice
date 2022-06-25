// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_BUFFER
#define CTHRICE_BUFFER

#include "error.c"
#include "string.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* restrict bgn;
    uint8_t* restrict end;
    uint8_t* restrict lst;
} Cthrice_Buffer;

Cthrice_Buffer cthrice_buffer_create(size_t len)
{
    uint8_t* restrict mem = malloc(len);
    if (!mem) {
        cthrice_error_alloc();
    }
    return (Cthrice_Buffer){.bgn = mem, .end = mem, .lst = mem + len};
}

Cthrice_Buffer cthrice_buffer_destroy(Cthrice_Buffer bfr)
{
    free(bfr.bgn);
    return (Cthrice_Buffer){.bgn = 0, .end = 0, .lst = 0};
}

size_t cthrice_buffer_size(Cthrice_Buffer bfr)
{
    return bfr.end - bfr.bgn;
}

size_t cthrice_buffer_capacity(Cthrice_Buffer bfr)
{
    return bfr.lst - bfr.bgn;
}

size_t cthrice_buffer_space(Cthrice_Buffer bfr)
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

Cthrice_Buffer cthrice_buffer_grow(Cthrice_Buffer bfr, size_t cap)
{
    uint8_t* restrict mem = realloc(bfr.bgn, cap);
    if (!mem) {
        cthrice_error_alloc();
    }
    bfr.lst = mem + cap;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_u8(Cthrice_Buffer bfr, uint8_t chr)
{
    if (cthrice_buffer_space(bfr) < 1) {
        bfr = cthrice_buffer_grow(bfr, 2 * cthrice_buffer_capacity(bfr));
    }

    *(bfr.end) = chr;
    bfr.end++;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_u64(Cthrice_Buffer bfr, uint64_t unt)
{
    const uint64_t BASE  = 10;
    uint64_t       place = BASE;

    while (1 != place) {
        if (unt > place) {
            place *= BASE;
            break;
        }
        place /= BASE;
        bfr = cthrice_buffer_append_u8(bfr, '0' + (uint8_t)(unt / place));
        unt %= place;
    }

    return bfr;
}

Cthrice_Buffer
cthrice_buffer_append_string(Cthrice_Buffer bfr, Cthrice_String str)
{
    size_t spc = cthrice_buffer_space(bfr);
    size_t len = cthrice_string_length(str);
    if (spc < len) {
        bfr = cthrice_buffer_grow(bfr, len - spc);
    }

    memcpy(bfr.end, str.bgn, len);
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_file(Cthrice_Buffer bfr, const char* name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        cthrice_error("Could not open file!");
    }

    const size_t CHUNK = 1024;
    size_t       wrt   = CHUNK;

    while (CHUNK == wrt) {
        size_t spc = cthrice_buffer_space(bfr);
        if (spc < CHUNK) {
            bfr = cthrice_buffer_grow(bfr, CHUNK - spc);
        }
        wrt = fread(bfr.end, 1, CHUNK, file);
        bfr.end += wrt;
    }

    if (!feof(file)) {
        cthrice_error("Problem while reading!");
    }

    fclose(file);
    return bfr;
}

#endif // CTHRICE_BUFFER
