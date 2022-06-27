// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_character.h"

bool cthrice_whitespace(uchr chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool cthrice_letter(uchr chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool cthrice_digit(uchr chr)
{
    return '0' <= chr && '9' >= chr;
}

bool cthrice_digit_bin(uchr chr)
{
    return '0' <= chr && '1' >= chr;
}

bool cthrice_digit_oct(uchr chr)
{
    return '0' <= chr && '7' >= chr;
}

bool cthrice_digit_hex(uchr chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}
