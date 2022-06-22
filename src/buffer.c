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
} ThriceBuffer;

ThriceBuffer ThriceBufferCreate(const size_t n)
{
    uint8_t* arr = malloc(n);
    if (!arr) {
        ThriceErrorAllocation();
    }
    return (ThriceBuffer){.beg = arr, .end = arr, .lst = arr + n};
}

ThriceBuffer ThriceBufferDestroy(const ThriceBuffer buf)
{
    free(buf.beg);
    return (ThriceBuffer){.beg = 0, .end = 0, .lst = 0};
}

size_t ThriceBufferSize(const ThriceBuffer buf)
{
    return buf.end - buf.beg;
}

size_t ThriceBufferCapacity(const ThriceBuffer buf)
{
    return buf.lst - buf.beg;
}

size_t ThriceBufferSpace(const ThriceBuffer buf)
{
    return buf.lst - buf.end;
}

ThriceString ThriceBufferView(ThriceBuffer buf)
{
    return (ThriceString){.beg = buf.beg, .end = buf.end};
}

ThriceBuffer ThriceBufferClear(ThriceBuffer buf)
{
    buf.end = buf.beg;
    return buf;
}

ThriceBuffer ThriceBufferGrow(ThriceBuffer buf, size_t cap)
{
    uint8_t* const restrict arr = realloc(buf.beg, cap);
    if (!arr) {
        ThriceErrorAllocation();
    }
    buf.lst = arr + cap;
    return buf;
}

ThriceBuffer ThriceBufferAppendU8(ThriceBuffer buf, const uint8_t chr)
{
    if (ThriceBufferSpace(buf) < 1) {
        buf = ThriceBufferGrow(buf, 2 * ThriceBufferCapacity(buf));
    }

    *(buf.end) = chr;
    buf.end++;
    return buf;
}

ThriceBuffer ThriceBufferAppendU64(ThriceBuffer buf, uint64_t u64)
{
    const uint64_t base  = 10;
    uint64_t       place = base;

    while (place != 1) {
        if (u64 > place) {
            place *= base;
            break;
        }
        place /= base;
        buf = ThriceBufferAppendU8(buf, '0' + (uint8_t)(u64 / place));
        u64 %= place;
    }

    return buf;
}

ThriceBuffer ThriceBufferAppendString(ThriceBuffer des, ThriceString src)
{
    const size_t spc = ThriceBufferSpace(des);
    const size_t len = ThriceStringLength(src);
    if (spc < len) {
        des = ThriceBufferGrow(des, len - spc);
    }

    memcpy(des.end, src.beg, len);
    return des;
}

ThriceBuffer ThriceBufferAppendFile(ThriceBuffer buf, const char* const name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        ThriceError("Could not open file!");
    }

    const size_t red = 1024;
    size_t       app = red;

    while (red == app) {
        const size_t spc = ThriceBufferSpace(buf);
        if (spc < red) {
            buf = ThriceBufferGrow(buf, red - spc);
        }
        app = fread(buf.end, 1, red, file);
        buf.end += app;
    }

    if (!feof(file)) {
        ThriceError("Problem while reading!");
    }

    fclose(file);
    return buf;
}

#endif // THRICE_BUFFER
