// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_BUFFER
#define THRICE_BUFFER

#include "error.c"
#include "string.c"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* restrict beg;
    uint8_t* restrict end;
    uint8_t* restrict lst;
} Buffer;

Buffer thriceBufferCreate(const size_t n)
{
    uint8_t* arr = malloc(n);
    if (!arr) {
        thriceErrorAllocation();
    }
    return (Buffer){.beg = arr, .end = arr, .lst = arr + n};
}

Buffer thriceBufferDestroy(const Buffer buf)
{
    free(buf.beg);
    return (Buffer){.beg = 0, .end = 0, .lst = 0};
}

size_t thriceBufferSize(const Buffer buf)
{
    return buf.end - buf.beg;
}

size_t thriceBufferCapacity(const Buffer buf)
{
    return buf.lst - buf.beg;
}

size_t thriceBufferSpace(const Buffer buf)
{
    return buf.lst - buf.end;
}

ThriceString thriceBufferView(Buffer buf)
{
    return (ThriceString){.beg = buf.beg, .end = buf.end};
}

Buffer thriceBufferClear(Buffer buf)
{
    buf.end = buf.beg;
    return buf;
}

Buffer thriceBufferGrow(Buffer buf, size_t cap)
{
    uint8_t* const restrict arr = realloc(buf.beg, cap);
    if (!arr) {
        thriceErrorAllocation();
    }
    buf.lst = arr + cap;
    return buf;
}

Buffer thriceBufferAppendU8(Buffer buf, const uint8_t chr)
{
    if (thriceBufferSpace(buf) < 1) {
        buf = thriceBufferGrow(buf, 2 * thriceBufferCapacity(buf));
    }

    *(buf.end) = chr;
    buf.end++;
    return buf;
}

Buffer thriceBufferAppendU64(Buffer buf, uint64_t u64)
{
    const uint64_t base  = 10;
    uint64_t       place = base;

    while (place != 1) {
        if (u64 > place) {
            place *= base;
            break;
        }
        place /= base;
        buf = thriceBufferAppendU8(buf, '0' + (uint8_t)(u64 / place));
        u64 %= place;
    }

    return buf;
}

Buffer thriceBufferAppendString(Buffer des, ThriceString src)
{
    const size_t spc = thriceBufferSpace(des);
    const size_t len = thriceStringLength(src);
    if (spc < len) {
        des = thriceBufferGrow(des, len - spc);
    }

    memcpy(des.end, src.beg, len);
    return des;
}

Buffer thriceBufferAppendFile(Buffer buf, const char* const name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        thriceError("Could not open file!");
    }

    const size_t red = 1024;
    size_t       app = red;

    while (red == app) {
        const size_t spc = thriceBufferSpace(buf);
        if (spc < red) {
            buf = thriceBufferGrow(buf, red - spc);
        }
        app = fread(buf.end, 1, red, file);
        buf.end += app;
    }

    if (!feof(file)) {
        thriceError("Problem while reading!");
    }

    fclose(file);
    return buf;
}

#endif // THRICE_BUFFER
