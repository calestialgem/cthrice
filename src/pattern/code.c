// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"

#include <stdlib.h>

ix ptrn_code_size(struct ptrnctx ctx)
{
    ASSERT(ctx.code.end >= ctx.code.bgn, "Code size is negative!");
    return ctx.code.end - ctx.code.bgn;
}

struct ptrnctx ptrn_code_put(struct ptrnctx ctx, struct ptrncode code)
{
    // Grow if the current avalible space is not enough.
    if (ctx.code.lst - ctx.code.end < 1) {
        // Store the current size to calculate the end pointer later.
        ix sze = ctx.code.end - ctx.code.bgn;
        ASSERT(sze >= 0, "Size is negative!");

        // Double the capacity.
        ix cap = ctx.code.lst - ctx.code.bgn;
        ASSERT(cap >= 0, "Capacity is negative");
        ix nwc = max(1, cap << 1);

        ASSERT(nwc > 0, "New code capacity is not positive!");
        struct ptrncode* mem =
            realloc(ctx.code.bgn, nwc * sizeof(struct ptrncode));
        CHECK(mem != null, "Could not allocate code!");

        ctx.code.bgn = mem;
        ctx.code.end = mem + sze;
        ctx.code.lst = mem + nwc;
    }

    *ctx.code.end++ = code;
    return ctx;
}

struct ptrnctx ptrn_code_destroy(struct ptrnctx ctx)
{
    free(ctx.code.bgn);
    ctx.code.bgn = ctx.code.end = ctx.code.lst = null;
    return ctx;
}
