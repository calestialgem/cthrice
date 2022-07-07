// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"

#include <stdio.h>
#include <stdlib.h>

void message(const byte* file, i32 line, const byte* type, const byte* msg)
{
    (void)fprintf(stderr, "%s:%d: %s: %s\n", file, line, type, msg);
}

void check(bool cnd, const byte* file, i32 line, const byte* msg)
{
    if (!cnd) {
        message(file, line, "error", msg);
        abort();
    }
}
