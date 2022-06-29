// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING_HH
#define CTHRICE_STRING_HH 1

#include "view.hh"

namespace cthrice
{
    /** Convert a static, null-terminated, C string. */
    View<char> cstr(const char* str);

    /** Parse the string to an unsigned integer. */
    uint64_t parse(View<char> str);

    /** Print the string before a new line. */
    void println(View<char> str);
} // namespace cthrice

#endif // CTHRICE_STRING_HH
