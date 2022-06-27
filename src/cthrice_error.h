// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_ERROR_H
#define CTHRICE_ERROR_H 1

#include "cthrice_types.h"

#include <stdio.h>
#include <stdlib.h>

#define CTHRICE_ERROR_LOGIC 1
#define CTHRICE_ERROR_ALLOC 2

void cthrice_error(ichr* msg);
void cthrice_error_alloc(void);

#endif // CTHRICE_ERROR_H
