// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"

#include <stdio.h>
#include <stdlib.h>

void message(const char* file, int line, const char* type, const char* msg)
{
    (void)fprintf(stderr, "%s:%d: %s: %s\n", file, line, type, msg);
}

void check(bool cnd, const char* file, int line, const char* msg)
{
    if (!cnd) {
        message(file, line, "error", msg);
        abort();
    }
}
