// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_FORMAT_H
#define CTHRICE_FORMAT_H 1

#include "cthrice_buffer.h"
#include "cthrice_character.h"
#include "cthrice_string.h"
#include "cthrice_types.h"

typedef union {
#define CTHRICE_FORMAT_FLAG_COUNT 5
    bool dat[CTHRICE_FORMAT_FLAG_COUNT];
    struct {
        bool left;
        bool plus;
        bool spce;
        bool altn;
        bool zero;
    };
} Cthrice_Format_Flags;

typedef struct {
    va_list              arp;
    Cthrice_Buffer       bfr;
    Cthrice_String       fmt;
    uchr*                crt;
    Cthrice_Format_Flags flg;
    unt32                wid;
    unt32                pre;
} Cthrice_Format_Context;

Cthrice_Format_Context cthrice_format_consume(Cthrice_Format_Context ctx);
Cthrice_Format_Context cthrice_format_skip(Cthrice_Format_Context ctx);
Cthrice_Format_Context cthrice_format_escape(Cthrice_Format_Context ctx);
Cthrice_Format_Context cthrice_format_flags(Cthrice_Format_Context ctx);
Cthrice_Format_Context
cthrice_format_number(Cthrice_Format_Context ctx, bool wid);
Cthrice_Format_Context cthrice_format_chr(Cthrice_Format_Context ctx);
Cthrice_Format_Context cthrice_format_string(Cthrice_Format_Context ctx);
Cthrice_Format_Context cthrice_format_context(Cthrice_Format_Context ctx);
Cthrice_Buffer cthrice_format_append(Cthrice_Buffer bfr, ichr* fmt, ...);
void           cthrice_format_println(ichr* fmt, ...);

#endif // CTHRICE_FORMAT_H
