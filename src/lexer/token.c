// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer/mod.h"
#include "internal.h"
#include "mod.h"

const Cthrice_Lexer* cthrice_lexer_create(const char* pth)
{
    Cthrice_String str = cthrice_buffer_append_file(pth);
}

void cthrice_lexer_destroy(const Cthrice_Lexer* lxr) {}
