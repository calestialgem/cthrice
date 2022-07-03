// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"

#include <stdio.h>

struct str str_convert(const byte* nts)
{
    struct str str = {.bgn = nts, .end = nts};
    while (*str.end) {
        str.end++;
    }
    return str;
}

ptr str_size(struct str str)
{
    ASSERT(str.end >= str.bgn, "String size is negative!");
    return str.end - str.bgn;
}

bool str_equal(struct str lhs, struct str rhs)
{
    ptr sze = str_size(lhs);
    if (sze != str_size(rhs)) {
        return false;
    }

    for (ptr i = 0; i < sze; i++) {
        if (lhs.bgn[i] != rhs.bgn[i]) {
            return false;
        }
    }

    return true;
}

const byte* str_find(struct str str, byte b)
{
    for (const byte* i = str.bgn; i < str.end; i++) {
        if (*i == b) {
            return i;
        }
    }
    return str.end;
}

bool str_contains(struct str str, byte b)
{
    return str_find(str, b) != str.end;
}

void println(struct str str)
{
    printf("%.*s\n", (int)str_size(str), str.bgn);
}
