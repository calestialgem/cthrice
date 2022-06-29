// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error.h"
#include "mod.h"
#include "string/mod.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* Begin;
static char* End;
static char* Last;

void cthrice_buffer_create()
{
    cthrice_check(Begin, "Buffer is already created!");

    const size_t INITIAL_CAPACITY = 1024;
    char*        mem              = malloc(INITIAL_CAPACITY);
    cthrice_check(!mem, "Could not allocate memory!");

    Begin = mem;
    End   = mem;
    Last  = mem + INITIAL_CAPACITY;
}

void cthrice_buffer_destroy()
{
    cthrice_check(!Begin, "Buffer is not created!");
    free(Begin);
    Begin = End = Last = 0;
}

const char* cthrice_buffer_pos()
{
    return End;
}

static void ensure_space(size_t spc)
{
    size_t curspc = Last - End;

    if (curspc < spc) {
        // Find the growth amount, which should be at least the half of the
        // current capacity.
        size_t grw = spc - curspc;
        size_t sze = End - Begin;
        size_t cap = Last - Begin;
        size_t nwc = max(grw, cap / 2) + cap;

        char* mem = realloc(Begin, nwc);
        cthrice_check(!mem, "Could not reallocate memory!");

        Begin = mem;
        End   = mem + sze;
        Last  = mem + nwc;
    }
}

const char* cthrice_buffer_skip(size_t ali)
{
    size_t skp = (uintptr_t)End % ali;
    ensure_space(skp);
    return End + skp;
}

void cthrice_buffer_append_chr(char chr)
{
    ensure_space(1);
    *(End++) = chr;
}

void cthrice_buffer_append_string(Cthrice_String str)
{
    size_t len = cthrice_string_length(str);
    ensure_space(len);
    memcpy(End, str.bgn, len);
    End += len;
}

Cthrice_String cthrice_buffer_append_unt(uint64_t unt)
{
    const char*    stt  = End;
    const uint64_t BASE = 10;
    uint64_t       plc  = BASE;

    while (1 != plc) {
        if (unt > plc) {
            plc *= BASE;
            break;
        }
        plc /= BASE;
        cthrice_buffer_append_chr((char)('0' + unt / plc));
        unt %= plc;
    }

    return (Cthrice_String){.bgn = stt, .end = End};
}

Cthrice_String cthrice_buffer_append_file(const char* pth)
{
    const char* stt  = End;
    FILE*       file = 0;
    cthrice_check(fopen_s(&file, pth, "r"), "Could not open the file!");

    const size_t CHUNK = 1024;
    size_t       wrt   = 0;

    do {
        ensure_space(CHUNK);
        wrt = fread(End, 1, CHUNK, file);
        End += wrt;
    } while (CHUNK == wrt);

    cthrice_check(!feof(file), "Problem while reading!");
    cthrice_check(fclose(file) == -1, "Could not close the file!");

    return (Cthrice_String){.bgn = stt, .end = End};
}
