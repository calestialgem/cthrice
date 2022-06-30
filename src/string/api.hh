// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_STRING_HH
#define CTHRICE_STRING_HH 1

#include "buffer.hh"
#include "range.hh"

#include <cstdint>

namespace cthrice
{
    /** Convert a static, null-terminated, C string to a character range. */
    Range<char> cstr(const char* str);

    /** Parse the character range to an unsigned integer. */
    uint64_t parse(Range<char> str);

    /** Print the character range before a new line. */
    void println(Range<char> str);

    /** Append the integer to the character buffer. */
    [[nodiscard]] Buffer<char> append(Buffer<char> bfr, uint64_t uint);

    /** Append the contents of the file to the character buffer. */
    [[nodiscard]] Buffer<char> append(Buffer<char> bfr, Range<char> path);
} // namespace cthrice

#endif // CTHRICE_STRING_HH
