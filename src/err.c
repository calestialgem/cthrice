// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_ERR
#define CTHR_ERR

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void cthr_err(const char* msg)
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

#endif // CTHR_ERR
