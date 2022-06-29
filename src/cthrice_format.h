// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_FORMAT_H
#define CTHRICE_FORMAT_H 1

#include "cthrice_buffer.h"
#include "cthrice_types.h"

Cthrice_Buffer cthrice_format_append(Cthrice_Buffer bfr, ichr* fmt, ...);
void           cthrice_format_println(ichr* fmt, ...);

#endif // CTHRICE_FORMAT_H
