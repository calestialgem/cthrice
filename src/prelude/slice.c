// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/api.h"

Slice slice_terminated(c8* a)
{
    Slice s = {.bgn = a, .end = a};
    while (*s.end) {
        s.end++;
    }
    return s;
}
