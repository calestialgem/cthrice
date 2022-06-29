// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"

namespace cthrice
{
    View<char> cstr(const char* str)
    {
        View<char> res = {.bgn = str, .end = str};
        while (*res.end != 0) {
            res.end++;
        }
        return res;
    }

    uint64_t parse(View<char> str)
    {
        uint64_t res = 0;
        for (const char* i = str.bgn; i < str.end; i++) {
            constexpr uint64_t BASE = 10;
            res *= BASE;
            res += *i - '0';
        }
        return res;
    }

    void println(View<char> str)
    {
        printf("%.*s\n", (int)size(str), str.bgn);
    }
} // namespace cthrice
