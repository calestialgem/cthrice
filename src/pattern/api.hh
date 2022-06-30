// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_PATTERN_HH
#define CTHRICE_PATTERN_HH 1

#include "buffer.hh"
#include "range.hh"
#include "string/api.hh"

namespace cthrice
{
    /** Compiled character pattern. */
    struct Pattern;

    /** Parse the pattern by searching for references in the pattern in the
     * buffer. */
    [[nodiscard]] Buffer<Pattern>
    parse(Buffer<Pattern> bfr, String name, String ptrn);

    /** Match the string to the patterns in the range. Returns the name of the
     * maching pattern. Returns empty string if nothing matches. */
    String match(Range<Pattern> rng, String str);
} // namespace cthrice

#endif // CTHRICE_PATTERN_HH
