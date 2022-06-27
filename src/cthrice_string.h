// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING_H
#define CTHRICE_STRING_H 1

#include "cthrice_character.h"
#include "cthrice_types.h"

typedef struct {
    uchr* bgn;
    uchr* end;
} Cthrice_String;

Cthrice_String cthrice_string_static(ichr* cstr);
uchr*          cthrice_string_first_pos_chr(Cthrice_String str, uchr chr);
uptr           cthrice_string_length(Cthrice_String str);
bool           cthrice_string_equals(Cthrice_String lhs, Cthrice_String rhs);
void           cthrice_string_println(Cthrice_String str);
Cthrice_String cthrice_string_trim(Cthrice_String str);
Cthrice_String cthrice_string_first_word(Cthrice_String str);
Cthrice_String cthrice_string_part(Cthrice_String str, uptr boff, uptr eoff);
Cthrice_String cthrice_string_skip(Cthrice_String str, uptr amt);
unt64          cthrice_string_parse_u64(Cthrice_String str);

#endif // CTHRICE_STRING_H
