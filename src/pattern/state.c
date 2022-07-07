// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"

#include <stdlib.h>

struct ptrnstates
ptrn_state_put(struct ptrnstates states, struct ptrnstate state)
{
    // Grow if the current avalible space is not enough.
    if (states.lst - states.end < 1) {
        // Store the current size to calculate the end pointer later.
        ptr sze = states.end - states.bgn;
        ASSERT(sze >= 0, "Size is negative!");

        // Double the capacity.
        ptr cap = states.lst - states.bgn;
        ASSERT(cap >= 0, "Capacity is negative");
        ptr nwc = max(1, cap << 1);

        struct ptrnstate* mem = realloc(states.bgn, nwc);
        CHECK(mem != null, "Could not allocate states!");

        states.bgn = mem;
        states.end = mem + sze;
        states.lst = mem + nwc;
    }

    *states.end++ = state;
    return states;
}

struct ptrnstates ptrn_state_clear(struct ptrnstates states)
{
    states.end = states.bgn;
    return states;
}

struct ptrnstates ptrn_state_destroy(struct ptrnstates states)
{
    free(states.bgn);
    states.bgn = states.end = states.lst = null;
    return states;
}
