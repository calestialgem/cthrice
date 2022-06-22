// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_ERROR
#define THRICE_ERROR

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define THRICE_ERROR_ALLOC 2

void thriceErrorAllocation(void)
{
    fprintf(stderr, "ERROR: Could not allocate memory!\n");
    exit(THRICE_ERROR_ALLOC);
}

void thriceError(const char* msg)
{
    static bool once = false;
    if (once) {
        fprintf(stderr, "ERROR: Called exit again!\n");
    } else {
        once = true;
        fprintf(stderr, "ERROR: %s\n", msg);
        exit(EXIT_FAILURE);
    }
}

#endif // THRICE_ERROR
