// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_BUF
#define CTHR_BUF

#include "err.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct buf {
    uint8_t* beg;
    uint8_t* end;
    uint8_t* lst;
};

struct buf cthr_buf_create(const size_t n)
{
    uint8_t* arr = malloc(n);
    if (!arr) {
        cthr_err("Could not allocate buffer!");
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

struct buf cthr_buf_clear(struct buf buf)
{
    buf.end = buf.beg;
    return buf;
}

struct buf cthr_buf_append_char(struct buf buf, const uint8_t chr)
{
    if (cthr_buf_space(buf) <= 0) {
        const size_t capacity = cthr_buf_capacity(buf);
        uint8_t*     arr      = realloc(buf.beg, capacity * 2);
        if (!arr) {
            cthr_err("Could not grow buffer!");
        }
        buf.lst = arr + capacity;
    }

    *(buf.end) = chr;
    buf.end++;
    return buf;
}

struct buf cthr_buf_append_file(struct buf buf, const char* const name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        cthr_err("Could not open file!");
    }

    int chr = fgetc(file);
    while (chr != EOF) {
        buf = cthr_buf_append_char(buf, chr);
        chr = fgetc(file);
    }

    if (!feof(file)) {
        cthr_err("Problem while reading!");
    }

    fclose(file);
    return buf;
}

#endif // CTHR_BUF
