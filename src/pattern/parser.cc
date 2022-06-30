// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"
#include "buffer.hh"
#include "internal.hh"

namespace cthrice
{
    Buffer<Pattern>
    parse(Buffer<Pattern> bfr, Range<char> name, Range<char> ptrn)
    {
        return put(bfr, {.name = name});
    }
} // namespace cthrice
