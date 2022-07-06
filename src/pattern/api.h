// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PATTERN_H
#define PATTERN_H 1

#include "buffer/api.h"
#include "string/api.h"
#include "types/api.h"

/* Pattern information. */
struct ptrninfo;
/* Compiled pattern byte code. */
struct ptrncode;

/* All pattern related data. */
struct ptrnctx {
    /* Buffer that holds the pattern names. */
    struct bfr bfr;
    /* Dynamic array of pattern name hash index. */
    struct {
        /* Pointer to the first allocated index. */
        ptr* bgn;
        /* Pointer to the index after the last valid and allocated one. */
        ptr* end;
    } hash;
    /* Dynamic array of pattern information. */
    struct {
        /* Pointer to the first allocated information. */
        struct ptrninfo* bgn;
        /* Pointer to the information after the last valid one. */
        struct ptrninfo* end;
        /* Pointer to the information after the last allocated one. */
        struct ptrninfo* lst;
    } info;
    /* Dynamic array of compiled pattern code. */
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
/* Deallocate the patterns. */
struct ptrnctx ptrn_destory(struct ptrnctx);

/* Match the pattern with the name to the input. Returns the initial portion of
 * the input that matched. Matches are checked from the begining. Empty match
 * means it did not match. */
struct srt ptrn_match(struct ptrnctx, struct str name, struct str input);

#endif // PATTERN_H
