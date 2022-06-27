// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_error.h"

#include <stdio.h>
#include <stdlib.h>

#define ERROR_LOGIC 1
#define ERROR_ALLOC 2

void cthrice_error(ichr* msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(ERROR_LOGIC);
}

void cthrice_error_alloc(void)
{
    fprintf(stderr, "ERROR: Could not allocate memory!\n");
    exit(ERROR_ALLOC);
}
