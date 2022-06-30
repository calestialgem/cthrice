// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "api.hh"
#include "buffer.hh"
#include "error.hh"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>

namespace cthrice
{
    Range<char> cstr(const char* str)
    {
        Range<char> res = {.bgn = str, .end = str};
        while (*res.end != 0) {
            res.end++;
        }
        return res;
    }

    uint64_t parse(Range<char> str)
    {
        uint64_t res = 0;
        for (const char* i = str.bgn; i < str.end; i++) {
            constexpr uint64_t BASE = 10;
            res *= BASE;
            res += *i - '0';
        }
        return res;
    }

    void println(Range<char> str)
    {
        printf("%.*s\n", (int)size(str), str.bgn);
    }

    Buffer<char> append(Buffer<char> bfr, uint64_t uint)
    {
        constexpr uint64_t BASE = 10;
        uint64_t           plc  = BASE;

        while (plc != 1) {
            if (uint > plc) {
                plc *= BASE;
                continue;
            }
            plc /= BASE;
            bfr = put(bfr, (char)('0' + uint / plc));
            uint %= plc;
        }

        return bfr;
    }

    Buffer<char> append(Buffer<char> bfr, Range<char> path)
    {
        std::ifstream file;
        file.open(path.bgn, std::ios::in);
        cthrice_check(!file.is_open(), "Could not open file!");

        auto len = file.seekg(0, std::ios::end).tellg();
        file.seekg(0, std::ios::beg);
        bfr = ensure_space(bfr, len);
        file.read(bfr.end, len);
        bfr.end += file.gcount();

        cthrice_check(file.bad(), "Could not read the file!");
        file.close();
        return bfr;
    }
} // namespace cthrice
