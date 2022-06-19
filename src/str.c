// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_STR
#define CTHR_STR

#include <stdint.h>

struct str {
    const uint8_t* beg;
    const uint8_t* end;
};

#endif // CTHR_STR
