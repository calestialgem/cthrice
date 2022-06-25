// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_ERROR
#define CTHRICE_ERROR

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CTHRICE_ERROR_LOGIC 1
#define CTHRICE_ERROR_ALLOC 2

void cthrice_error(const char* msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(CTHRICE_ERROR_LOGIC);
}

void cthrice_error_alloc(void)
{
    fprintf(stderr, "ERROR: Could not allocate memory!\n");
    exit(CTHRICE_ERROR_ALLOC);
}

#endif // CTHRICE_ERROR
