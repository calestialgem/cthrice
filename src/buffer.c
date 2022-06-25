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

Cthrice_Buffer cthrice_buffer_create(const size_t n)
{
    uint8_t* arr = malloc(n);
    if (!arr) {
        cthrice_error_alloc();
    }
    return (Cthrice_Buffer){.bgn = arr, .end = arr, .lst = arr + n};
}

Cthrice_Buffer cthrice_buffer_destroy(const Cthrice_Buffer buf)
{
    free(buf.bgn);
    return (Cthrice_Buffer){.bgn = 0, .end = 0, .lst = 0};
}

size_t cthrice_buffer_size(const Cthrice_Buffer buf)
{
    return buf.end - buf.bgn;
}

size_t cthrice_buffer_capacity(const Cthrice_Buffer buf)
{
    return buf.lst - buf.bgn;
}

size_t cthrice_buffer_space(const Cthrice_Buffer buf)
{
    return buf.lst - buf.end;
}

Cthrice_String cthrice_buffer_view(Cthrice_Buffer buf)
{
    return (Cthrice_String){.bgn = buf.bgn, .end = buf.end};
}

Cthrice_Buffer cthrice_buffer_clear(Cthrice_Buffer buf)
{
    buf.end = buf.bgn;
    return buf;
}

Cthrice_Buffer cthrice_buffer_grow(Cthrice_Buffer buf, size_t cap)
{
    uint8_t* const restrict arr = realloc(buf.bgn, cap);
    if (!arr) {
        cthrice_error_alloc();
    }
    buf.lst = arr + cap;
    return buf;
}

Cthrice_Buffer cthrice_buffer_append_u8(Cthrice_Buffer buf, const uint8_t chr)
{
    if (cthrice_buffer_space(buf) < 1) {
        buf = cthrice_buffer_grow(buf, 2 * cthrice_buffer_capacity(buf));
    }

    *(buf.end) = chr;
    buf.end++;
    return buf;
}

Cthrice_Buffer cthrice_buffer_append_u64(Cthrice_Buffer buf, uint64_t u64)
{
    const uint64_t base  = 10;
    uint64_t       place = base;

    while (place != 1) {
        if (u64 > place) {
            place *= base;
            break;
        }
        place /= base;
        buf = cthrice_buffer_append_u8(buf, '0' + (uint8_t)(u64 / place));
        u64 %= place;
    }

    return buf;
}

Cthrice_Buffer
cthrice_buffer_append_string(Cthrice_Buffer des, Cthrice_String src)
{
    const size_t spc = cthrice_buffer_space(des);
    const size_t len = cthrice_string_length(src);
    if (spc < len) {
        des = cthrice_buffer_grow(des, len - spc);
    }

    memcpy(des.end, src.bgn, len);
    return des;
}

Cthrice_Buffer
cthrice_buffer_append_file(Cthrice_Buffer buf, const char* const name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        cthrice_error("Could not open file!");
    }

    const size_t READ_LENGTH = 1024;
    size_t       app         = READ_LENGTH;

    while (READ_LENGTH == app) {
        const size_t spc = cthrice_buffer_space(buf);
        if (spc < READ_LENGTH) {
            buf = cthrice_buffer_grow(buf, READ_LENGTH - spc);
        }
        app = fread(buf.end, 1, READ_LENGTH, file);
        buf.end += app;
    }

    if (!feof(file)) {
        cthrice_error("Problem while reading!");
    }

    fclose(file);
    return buf;
}

#endif // CTHRICE_BUFFER
