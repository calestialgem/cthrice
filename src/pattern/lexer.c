// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "internal.h"
#include "types/api.h"

#include <stdlib.h>

struct ptrnlex ptrn_lex(struct str) {}

struct ptrnlex ptrn_lex_destroy(struct ptrnlex lex)
{
    free(lex.bgn);
    lex.bgn = lex.end = lex.lst = null;
    return lex;
}
