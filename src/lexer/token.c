// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer/mod.h"
#include "internal.h"
#include "mod.h"

/*
 * x: 'a'       -> x is the character a
 * x: 'a' - 'b' -> x is any character in range [a, b]
 * x: a b       -> x is a then b
 * x: a
 *    b         -> x is a then b
 * x: a | b     -> x is a or b
 * x: [a]       -> x is a or nothing
 * x: {a}       -> x is 0 or more a
 * x: (a)       -> x is 1 or more a
 * f<a, b>:     -> f is a template taking a and b as parameters
 * x: f<a, b>   -> x is result of template f using a and b as arguments
 * x@:          -> x is a token
 */

const Cthrice_Lexer* cthrice_lexer_create(const char* pth)
{
    Cthrice_String str = cthrice_buffer_append_file(pth);
}

void cthrice_lexer_destroy(const Cthrice_Lexer* lxr) {}
