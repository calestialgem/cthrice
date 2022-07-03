// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PATTERN_H
#define PATTERN_H 1

#include "buffer/api.h"
#include "string/api.h"

/* Compiled pattern. */
struct ptrn;

/* Dynamic array of patterns. */
struct ptrns {
    /* Buffer that holds the pattern names. */
    struct bfr bfr;
    /* Pointer to the first allocated pattern. */
    struct ptrn* bgn;
    /* Pointer to the pattern after the last valid one. */
    struct ptrn* end;
    /* Pointer to the pattern after the last allocated one. */
    struct ptrn* lst;
};

/* Parse the pattern by searching for references in the patterns. */
struct ptrns ptrn_parse(struct ptrns, struct str);

/* Match the string to the patterns. Returns the name of the maching pattern.
 * Returns empty string if nothing matches. */
struct str ptrn_match(struct ptrns, struct str);

/* Deallocate the patterns. */
struct ptrns ptrn_destory(struct ptrns);

#endif // PATTERN_H
