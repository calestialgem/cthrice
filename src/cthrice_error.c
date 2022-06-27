// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_error.h"

void cthrice_error(ichr* msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(CTHRICE_ERROR_LOGIC);
}

void cthrice_error_alloc(void)
{
    fprintf(stderr, "ERROR: Could not allocate memory!\n");
    exit(CTHRICE_ERROR_ALLOC);
}
