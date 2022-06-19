// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_BUF
#define CTHR_BUF

#include <err.h>
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
    uint8_t* a = malloc(n);
    if (!a) {
        err(EXIT_FAILURE, "Could not allocate buffer!");
    }
    return (struct buf){.beg = a, .end = a, .lst = a + n};
}

void cthr_buf_destroy(const struct buf b)
{
    free(b.beg);
}

size_t cthr_buf_size(const struct buf b)
{
    return b.end - b.beg;
}

size_t cthr_buf_capacity(const struct buf b)
{
    return b.lst - b.beg;
}

size_t cthr_buf_space(const struct buf b)
{
    return b.lst - b.end;
}

struct buf cthr_buf_append_char(struct buf b, const uint8_t c)
{
    if (cthr_buf_space(b) <= 0) {
        const size_t capacity = cthr_buf_capacity(b);
        uint8_t*     a        = realloc(b.beg, capacity * 2);
        if (!a) {
            err(EXIT_FAILURE, "Could not grow buffer!");
        }
        b.lst = a + capacity;
    }

    *(b.end) = c;
    b.end++;
    return b;
}

struct buf cthr_buf_append_file(struct buf b, const char* const name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        err(EXIT_FAILURE, "Cannot open file %s!", name);
    }

    int c = fgetc(file);
    do {
        b = cthr_buf_append_char(b, c);
        c = fgetc(file);
    } while (c != EOF);

    fclose(file);
    return b;
}

#endif // CTHR_BUF
