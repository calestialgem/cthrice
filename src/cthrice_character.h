// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_CHARACTER_H
#define CTHRICE_CHARACTER_H 1

#include "cthrice_types.h"

bool cthrice_whitespace(uchr chr);
bool cthrice_letter(uchr chr);
bool cthrice_digit(uchr chr);
bool cthrice_digit_bin(uchr chr);
bool cthrice_digit_oct(uchr chr);
bool cthrice_digit_hex(uchr chr);

#endif // CTHRICE_CHARACTER_H
