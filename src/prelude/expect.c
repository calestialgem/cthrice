// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/api.h"

#include <stdio.h>
#include <stdlib.h>

void expectsl(b8 cnd, const c8* msg, const c8* file, u32 line)
{
    if (!cnd) {
        printf("%s:%u: error: %s", file, line, msg);
        abort();
    }
}

void unexpectedsl(const c8* msg, const c8* file, u32 line)
{
    expectsl(false, msg, file, line);
}
