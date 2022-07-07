// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"

#include <stdlib.h>

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

    // If the hash is empty, probably the info is empty too.
    if (sze == 0) {
        // Actually it is supposed to be.
        ASSERT(
            ctx.info.end - ctx.info.bgn == 0,
            "Hashes are empty but not the infos!");
        return (struct range){.bgn = ctx.info.bgn, .end = ctx.info.end};
    }

    // Find the indicies that correspond to this and the next hashes.
    ptr* this = ctx.hash.bgn + hash(name) % sze;
    ptr* next = this + 1;

    struct ptrninfo* bgn = ctx.info.bgn + *this;
    struct ptrninfo* end = ctx.info.end;

    // This hash might be the last one; thus, check the next one.
    // If the next hash is valid limit the search range from the end.
    if (next < ctx.hash.end) {
        end = ctx.info.bgn + *next;
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

struct ptrnctx ptrn_hash_put(struct ptrnctx ctx, struct ptrninfo info)
{
    return ctx;
}

struct ptrnctx ptrn_hash_destroy(struct ptrnctx ctx)
{
    free(ctx.hash.bgn);
    free(ctx.info.bgn);
    ctx.hash.bgn = ctx.hash.end = null;
    ctx.info.bgn = ctx.info.end = ctx.info.lst = null;
    return ctx;
}
