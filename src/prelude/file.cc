// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "list.cc"
#include "types.cc"
#include "view.cc"

#include <cstdio>
#include <tuple>

namespace cthrice::file
{
    /* Load the contents of a file to the buffer. Returns the resulting buffer
     * and a view to the contents of the file. */
    [[nodiscard]] std::tuple<List<b8>, View<b8>>
    load(List<b8> bfr, const b8* path)
    {
        std::FILE* file = std::fopen(path, "r");
        check(file != nullptr, "Could not open the file!");

        ix           bix   = List<b8>::size(bfr);
        constexpr ix CHUNK = 1024;
        ix           wrt   = 0;

        do {
            bfr = List<b8>::ensure_space(bfr, CHUNK);
            wrt = (ix)std::fread(bfr.end, 1, CHUNK, file);
            bfr.end += wrt;
        } while (CHUNK == wrt);

        check(std::feof(file) != 0, "Problem while reading!");
        check(std::fclose(file) != -1, "Could not close the file!");

        return std::make_tuple(bfr, List<b8>::view_end(bfr, bix));
    }
} // namespace cthrice::file
