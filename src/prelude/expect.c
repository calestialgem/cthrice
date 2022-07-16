// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* If the condition does not hold, print the error message with the
 * file and line information and abort. */
void cthrice_expect_source_location(
    bool         condition,
    char const*  message,
    char const*  file,
    unsigned int line)
{
    if (!condition) {
        fprintf(stderr, "%s:%u: error: %s\n", file, line, message);
        abort();
    }
}

/* If the condition does not hold, print the error message with the file and
 * line information and abort. Takes the file and line information itself. */
#define cthrice_expect(condition, message) \
    cthrice_expect_source_location(condition, message, __FILE__, __LINE__)
