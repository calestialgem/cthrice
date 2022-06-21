// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_BUF
#define CTHR_BUF

#include "err.c"
#include "str.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buf {
    uint8_t* restrict beg;
    uint8_t* restrict end;
    uint8_t* restrict lst;
};

struct buf cthr_buf_create(const size_t n)
{
    uint8_t* arr = malloc(n);
    if (!arr) {
        cthr_alloc_err();
    }
    return (struct buf){.beg = arr, .end = arr, .lst = arr + n};
}

struct buf cthr_buf_destroy(const struct buf buf)
{
    free(buf.beg);
    return (struct buf){.beg = 0, .end = 0, .lst = 0};
}

size_t cthr_buf_size(const struct buf buf)
{
    return buf.end - buf.beg;
}

size_t cthr_buf_capacity(const struct buf buf)
{
    return buf.lst - buf.beg;
}

size_t cthr_buf_space(const struct buf buf)
{
    return buf.lst - buf.end;
}

struct str cthr_buf_view(struct buf buf)
{
    return (struct str){.beg = buf.beg, .end = buf.end};
}

struct buf cthr_buf_clear(struct buf buf)
{
    buf.end = buf.beg;
    return buf;
}

struct buf cthr_buf_grow(struct buf buf, size_t cap)
{
    uint8_t* const restrict arr = realloc(buf.beg, cap);
    if (!arr) {
        cthr_alloc_err();
    }
    buf.lst = arr + cap;
    return buf;
}

struct buf cthr_buf_u8(struct buf buf, const uint8_t chr)
{
    if (cthr_buf_space(buf) < 1) {
        buf = cthr_buf_grow(buf, 2 * cthr_buf_capacity(buf));
    }

    *(buf.end) = chr;
    buf.end++;
    return buf;
}

struct buf cthr_buf_u64(struct buf buf, uint64_t u64)
{
    const uint64_t base  = 10;
    uint64_t       place = base;

    while (place != 1) {
        if (u64 > place) {
            place *= base;
            break;
        }
        place /= base;
        buf = cthr_buf_u8(buf, '0' + u64 / place);
        u64 %= place;
    }

    return buf;
}

struct buf cthr_buf_str(struct buf des, struct str src)
{
    const size_t spc = cthr_buf_space(des);
    const size_t len = cthr_str_length(src);
    if (spc < len) {
        des = cthr_buf_grow(des, len - spc);
    }

    memcpy(des.end, src.beg, len);
    return des;
}

struct buf cthr_buf_file(struct buf buf, const char* const name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        cthr_err("Could not open file!");
    }

    const size_t red = 1024;
    size_t       app = red;

    while (red == app) {
        const size_t spc = cthr_buf_space(buf);
        if (spc < red) {
            buf = cthr_buf_grow(buf, red - spc);
        }
        app = fread(buf.end, 1, red, file);
        buf.end += app;
    }

    if (!feof(file)) {
        cthr_err("Problem while reading!");
    }

    fclose(file);
    return buf;
}

#endif // CTHR_BUF
