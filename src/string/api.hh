// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING_HH
#define CTHRICE_STRING_HH 1

#include "buffer.hh"
#include "range.hh"

#include <cstdint>

namespace cthrice
{
    /** Range of characters. */
    using String = Range<char>;

    /** Convert a static, null-terminated, C string to a string. */
    String cstr(const char* str);

    /** Parse the string to an unsigned integer. */
    uint64_t parse(String str);

    /** Print the string before a new line. */
    void println(String str);

    /** Append the integer to the character buffer. */
    [[nodiscard]] Buffer<char> append(Buffer<char> bfr, uint64_t uint);

    /** Append the contents of the file to the character buffer. */
    [[nodiscard]] Buffer<char> append(Buffer<char> bfr, String path);
} // namespace cthrice

#endif // CTHRICE_STRING_HH
