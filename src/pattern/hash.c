// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.h"
#include "error/api.h"
#include "internal.h"
#include "string/api.h"

#include <stdlib.h>
#include <string.h>

const ptr PTRN_INVALID_NAME = -1;

/* Range of pattern information that coresponds to a hash. The size of the range
 * gives the collision amount for the corresponding hash. */
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

/* Get the range corresponding to the hash. */
static struct range range(struct ptrnctx ctx, ptr hash)
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
    ptr* this = ctx.hash.bgn + hash % sze;
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
    struct range rnge = range(ctx, hash(name));

    for (struct ptrninfo* i = rnge.bgn; i < rnge.end; i++) {
        if (str_equal(i->name, name)) {
            return i->code;
        }
    }

    return PTRN_INVALID_NAME;
}

static const ptr MAX_COLLISION = 8;

static bool need_rehash(struct ptrnctx ctx)
{
    ptr sze = ctx.hash.end - ctx.hash.bgn;
    for (ptr i = 0; i < sze; i++) {
        struct range rnge = range(ctx, i);
        if (rnge.end - rnge.bgn > MAX_COLLISION) {
            return true;
        }
    }
    return false;
}

/* Compare the pattern infos by the hashes of their names. */
static int compare(const void* vlhs, const void* vrhs)
{
    const struct ptrninfo* lhs = vlhs;
    const struct ptrninfo* rhs = vrhs;
    return (int)(hash(lhs->name) - hash(rhs->name));
}

/* Rehash the map until the collisions are small. */
static struct ptrnctx rehash(struct ptrnctx ctx)
{
    do {
        // Grow the hash array.
        ptr sze = ctx.hash.end - ctx.hash.bgn;
        ASSERT(sze >= 0, "Size is negative!");
        ptr nwc = sze << 1;

        ptr* mem = realloc(ctx.hash.bgn, nwc);
        CHECK(mem != null, "Could not allocate hash!");

        ctx.hash.bgn = mem;
        ctx.hash.end = mem + nwc;

        // Sort the infos using hash.
        qsort(
            ctx.info.bgn,
            ctx.info.end - ctx.info.bgn,
            sizeof(struct ptrninfo),
            &compare);

        // TODO: Fix pointers from hashes.
    } while (need_rehash(ctx));
    return ctx;
}

struct ptrnctx ptrn_hash_put(struct ptrnctx ctx, struct ptrninfo info)
{
    // Get the range for the corresponding hash.
    ptr          hsh  = hash(info.name);
    struct range rnge = range(ctx, hsh);

    // Check if it already exists.
    for (struct ptrninfo* i = rnge.bgn; i < rnge.end; i++) {
        CHECK(str_equal(i->name, info.name), "Name already exists!");
    }

    // Add to the map.

    // Grow if the current avalible space is not enough.
    if (ctx.info.lst - ctx.info.end < 1) {
        // Store the current size to calculate the end pointer later.
        ptr sze = ctx.info.end - ctx.info.bgn;
        ASSERT(sze >= 0, "Size is negative!");

        // Double the capacity.
        ptr cap = ctx.info.lst - ctx.info.bgn;
        ASSERT(cap >= 0, "Capacity is negative");
        ptr nwc = cap << 1;

        struct ptrninfo* mem = realloc(ctx.info.bgn, nwc);
        CHECK(mem != null, "Could not allocate info!");

        ctx.info.bgn = mem;
        ctx.info.end = mem + sze;
        ctx.info.lst = mem + nwc;
    }

    // If it would not create too many collisions.
    if (rnge.end - rnge.bgn <= MAX_COLLISION) {
        // Free the end of the range by moving everything after it.
        // Use memmove instead of memcpy because it collides.
        memmove(rnge.end + 1, rnge.end, ctx.info.end - rnge.end);
        *rnge.end++ = info;
    } else {
        // Just add to the end; rehash will position it correctly.
        *ctx.info.end++ = info;
        ctx             = rehash(ctx);
    }

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
