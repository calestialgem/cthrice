// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mod.h"

bool cthrice_whitespace(char chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool cthrice_letter(char chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool cthrice_digit(char chr)
{
    return '0' <= chr && '9' >= chr;
}

bool cthrice_digit_bin(char chr)
{
    return '0' <= chr && '1' >= chr;
}

bool cthrice_digit_oct(char chr)
{
    return '0' <= chr && '7' >= chr;
}

bool cthrice_digit_hex(char chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}
