// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PATTERN_H
#define PATTERN_H 1

#include "buffer/api.h"
#include "string/api.h"
#include "types/api.h"

/* Compiled pattern byte code. */
struct ptrncode;

/* Pattern information. */
struct ptrninfo {
    /* Identifier. */
    struct str name;
    /* Index of the start of the pattern's code. */
    ptr code;
    /* Whether the pattern matches in the nameless search. */
    bool visible;
};

/* All pattern related data. */
struct ptrnctx {
    /* Buffer that holds the pattern names. */
    struct bfr bfr;

    /* Fields related to pattern name hash indicies. */
    struct {
        /* Pointer to the first allocated hash index. */
        ptr* bgn;
        /* Pointer to the hash index after the last valid and allocated one. */
        ptr* end;
    } hash;

    /* Fields related to pattern information. */
    struct {
        /* Pointer to the first allocated pattern information. */
        struct ptrninfo* bgn;
        /* Pointer to the pattern information after the last valid one. */
        struct ptrninfo* end;
        /* Pointer to the pattern information after the last allocated one. */
        struct ptrninfo* lst;
    } info;

    /* Fields related to compiled pattern code. */
    struct {
        /* Pointer to the first allocated code. */
        struct ptrncode* bgn;
        /* Pointer to the code after the last valid one. */
        struct ptrncode* end;
        /* Pointer to the code after the last allocated one. */
        struct ptrncode* lst;
    } code;
};

/* Parse the pattern by searching for references in the patterns. */
struct ptrnctx ptrn_parse(struct ptrnctx, struct str);

/* Match the string. Returns the name of a maching pattern. Returns empty string
 * if nothing matches. If the string matches to multiple patterns the one
 * returned is undeterministic. */
struct str ptrn_match(struct ptrnctx, struct str);

/* Check the string to the pattern with the give name. */
bool ptrn_check(struct ptrnctx, struct str, struct str);

/* Deallocate the patterns. */
struct ptrnctx ptrn_destory(struct ptrnctx);

#endif // PATTERN_H
