// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

struct str ptrn_match(struct ptrnctx ctx, struct str name, struct str input)
{
    ix code = ptrn_hash_get(ctx, name);

    if (code == PTRN_INVALID_NAME) {
        return (struct str){0};
    }

    struct ptrnstate init = {.input = input, .code = code, .dead = false};
    return ptrn_decode(ctx, init);
}
