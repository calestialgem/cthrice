// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_BUFFER_H
#define CTHRICE_BUFFER_H 1

#include "cthrice_string.h"
#include "cthrice_types.h"

typedef struct {
    unt8* restrict bgn;
    unt8* restrict end;
    unt8* restrict lst;
} Cthrice_Buffer;

Cthrice_Buffer cthrice_buffer_create(uptr cap);
Cthrice_Buffer cthrice_buffer_destroy(Cthrice_Buffer bfr);
uptr           cthrice_buffer_size(Cthrice_Buffer bfr);
uptr           cthrice_buffer_capacity(Cthrice_Buffer bfr);
uptr           cthrice_buffer_space(Cthrice_Buffer bfr);
Cthrice_String cthrice_buffer_view(Cthrice_Buffer bfr);
Cthrice_Buffer cthrice_buffer_clear(Cthrice_Buffer bfr);
Cthrice_Buffer cthrice_buffer_recap(Cthrice_Buffer bfr, uptr cap);
Cthrice_Buffer cthrice_buffer_append_unt8(Cthrice_Buffer bfr, unt8 chr);
Cthrice_Buffer cthrice_buffer_append_unt64(Cthrice_Buffer bfr, unt64 unt);
Cthrice_Buffer
cthrice_buffer_append_string(Cthrice_Buffer bfr, Cthrice_String str);
Cthrice_Buffer cthrice_buffer_append_file(Cthrice_Buffer bfr, ichr* name);

#endif // CTHRICE_BUFFER_H
