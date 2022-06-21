// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_FMT
#define CTHR_FMT

#include "buf.c"
#include "err.c"
#include "str.c"

#include <stdarg.h>

struct buf cthr_fmt_append(struct buf buf, struct str fmt, ...)
{
    va_list argp = 0;
    va_start(argp, fmt);

    const struct str mds  = cthr_str_c("cds");
    bool             inm  = false;
    uint32_t         wid  = 0;
    const uint8_t    meta = '%';

    for (;;) {
        const int8_t* const start = cthr_str_chr_first(fmt, meta);
    }

    va_end(argp);
    return buf;
}

#endif // CTHR_FMT
