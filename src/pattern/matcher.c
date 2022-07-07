// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"
#include "types/api.h"

#include <stdio.h>
#include <stdlib.h>

struct str ptrn_match(struct ptrnctx ctx, struct str name, struct str input)
{
    // TODO: Find pattern using the hash map.
    for (const struct ptrninfo* i = ctx.info.bgn; i < ctx.info.end; i++) {
        if (str_equal(i->name, name)) {
            // Create the initial state.
            struct ptrnstate init = {
                .input = input,
                .code  = i->code,
                .dead  = false};

            // If matches the pattern, return the match.
            struct str match = ptrn_decode_all(ctx, init);
            if (str_finite(match)) {
                return match;
            }
        }
    }

    // Return empty string if nothing matched.
    return (struct str){0};
}
