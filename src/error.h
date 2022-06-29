// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_ERROR_H
#define CTHRICE_ERROR_H 1

#include <stdint.h>
#include <stdio.h>

void cthrice_error(const char* file, uint64_t line, const char* msg)
{
    fprintf(stderr, "%s:%llu: error: %s\n", file, line, msg);
    exit(1);
}

#define cthrice_check(cnd, msg)                     \
    do {                                            \
        if (cnd) {                                  \
            cthrice_error(__FILE__, __LINE__, msg); \
        }                                           \
    } while (0)

#endif // CTHRICE_ERROR_H
