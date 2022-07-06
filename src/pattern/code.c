// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"

ptr code_size(struct ptrnctx ctx)
{
    ASSERT(ctx.code.end >= ctx.code.bgn, "Negative code size!");
    return ctx.code.end - ctx.code.bgn;
}

struct decoderes
decode(struct ptrnctx ctx, struct states states, struct state state)
{
    ASSERT(state.code >= 0, "Code out of bounds!");
    ASSERT(state.code < code_size(ctx), "Code out of bounds!");
    struct ptrncode code = *(ctx.code.bgn + state.code);

    switch (code.type) {
        case DIVERGE:
            ASSERT(code.amt > 0, "Nonpositive amount!");
            ASSERT(
                state.code + code.amt < code_size(ctx),
                "Divergence out of bounds!");
            for (ptr j = 0; j < code.amt; j++) {
                state.code++;
                states = state_put(states, state_step(ctx, state));
            }
            break;
        case MATCH:
            return (struct decoderes){.states = states, .matched = true};
        default:
            states = state_put(states, state_step(ctx, state));
    }

    return (struct decoderes){.states = states, .matched = false};
}
