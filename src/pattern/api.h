// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PATTERN_H
#define PATTERN_H 1

#include "buffer/api.h"
#include "string/api.h"

/* Compiled pattern byte code. */
struct ptrncode;

/* Pattern information. */
struct ptrn {
    /* Identifier. */
    struct str name;
    /* Start of the code. */
    struct ptrncode* code;
};

/* Dynamic array of patterns. */
struct ptrns {
    /* Buffer that holds the pattern names. */
    struct bfr bfr;

    /* Fields related to pattern name hashes. */
    struct {
        /* Pointer to the first allocated hash. */
        struct ptrn** bgn;
        /* Pointer to the hash after the last valid and allocated one. */
        struct ptrn** end;
    } hash;

    /* Fields related to pattern information. */
    struct {
        /* Pointer to the first allocated pattern. */
        struct ptrn* bgn;
        /* Pointer to the pattern after the last valid one. */
        struct ptrn* end;
        /* Pointer to the pattern after the last allocated one. */
        struct ptrn* lst;
    } values;

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
struct ptrns ptrn_parse(struct ptrns, struct str);

/* Match the string to the patterns. Returns the name of a maching pattern.
 * Returns empty string if nothing matches. If the string matches to multiple
 * patterns the one returned is undeterministic. */
struct str ptrn_match(struct ptrns, struct str);

/* Check the string to the pattern with the give name. */
bool ptrn_check(struct ptrns, struct str, struct str);

/* Deallocate the patterns. */
struct ptrns ptrn_destory(struct ptrns);

#endif // PATTERN_H
