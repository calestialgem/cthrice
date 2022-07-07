// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"

const ptr PTRN_INVALID_NAME = -1;

/* Range of pattern information that coresponds to a hash. */
struct range {
    /* Pointer to the first information. */
    struct ptrninfo* bgn;
    /* Pointer to the information after the last one. */
    struct ptrninfo* end;
};

/* Hash the pattern name. Assumes the pattern name is made of English alphabet
 * with the same case throughout the string. */
static ptr hash(struct str str)
{
    ptr res = 0;

    for (const byte* i = str.bgn; i < str.end; i++) {
        const ptr prime = 31;
        res *= prime;
        res += *i;
    }

    return res;
}

/* Get the range corresponding to the hash of the name. */
static struct range range(struct ptrnctx ctx, struct str name)
{
    ptr sze = ctx.hash.end - ctx.hash.bgn;
    ASSERT(sze >= 0, "Hash size is negative!");

    ptr* hashbgn = ctx.hash.bgn + hash(name) % sze;
    ptr* hashend = hashbgn + 1;

    struct ptrninfo* bgn = ctx.info.bgn + *hashbgn;
    struct ptrninfo* end = ctx.info.end;

    if (hashend < ctx.hash.end) {
        end = ctx.info.bgn + *hashend;
    }

    return (struct range){.bgn = bgn, .end = end};
}

ptr ptrn_hash_get(struct ptrnctx ctx, struct str name)
{
    struct range rnge = range(ctx, name);

    for (struct ptrninfo* i = rnge.bgn; i < rnge.end; i++) {
        if (str_equal(i->name, name)) {
            return i->code;
        }
    }

    return PTRN_INVALID_NAME;
}

struct ptrnctx ptrn_hash_put(struct ptrnctx, struct ptrninfo) {}

struct ptrnctx ptrn_hash_destroy(struct ptrnctx) {}
