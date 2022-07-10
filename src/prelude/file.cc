// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "error.cc"
#include "list.cc"
#include "types.cc"
#include "view.cc"

#include <cstdio>

namespace cthrice::file
{
    /* Result of loading a file to the memory. */
    struct Result {
        /* Memory buffer that the contents of the file were loaded. */
        List<B8> bfr;
        /* Contents of the file. */
        View<const B8> file;
    };

    /* Load the contents of a file to the buffer. Returns the resulting buffer
     * and a view to the contents of the file. */
    [[nodiscard]] Result load(List<B8> bfr, const B8* path)
    {
        std::FILE* file = std::fopen(path, "r");
        check(file != nullptr, "Could not open the file!");

        Ix           bix   = list::size(bfr);
        constexpr Ix CHUNK = 1024;
        Ix           wrt   = 0;

        do {
            bfr = list::reserve(bfr, CHUNK);
            wrt = (Ix)std::fread(bfr.end, 1, CHUNK, file);
            bfr.end += wrt;
        } while (CHUNK == wrt);

        check(std::feof(file) != 0, "Problem while reading!");
        check(std::fclose(file) != -1, "Could not close the file!");

        return {.bfr = bfr, .file = list::view_end(bfr, bix)};
    }
} // namespace cthrice::file
