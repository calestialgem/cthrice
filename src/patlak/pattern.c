// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/scalar.c"
#include "prelude/string.c"

/* Pattern information. */
typedef struct {
    /* Name of the pattern. */
    CTString name;
    /* Index to the start of the pattern's code. */
    CTIndex start;
} CTPatlakPattern;

/* Hash map of all the patterns. */
typedef struct {
    /* Pattern information. */
    struct {
        /* Border before the first information. */
        CTPatlakPattern* first;
        /* Border after the last information. */
        CTPatlakPattern* last;
        /* Border after the last allocated information. */
        CTPatlakPattern* allocated;
    } information;
    /* Pattern information indicies corresponding to pattern name hashes. */
    struct {
        /* Border before the first index. */
        CTIndex* first;
        /* Border before the last index. */
        CTIndex* last;
    } indicies;
} CTPatlakPatterns;
