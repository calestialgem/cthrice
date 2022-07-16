// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/list.cc"
#include "prelude/range.cc"
#include "prelude/scalar.cc"
#include "prelude/split.cc"
#include "prelude/view.cc"

#include <cstdio>

namespace cthrice
{
/* Load the contents of the file at the given path to the buffer. Returns a view
 * to the loaded contents. */
[[nodiscard]] constexpr View<char>
load(List<char>& buffer, char const* const path) noexcept
{
    std::FILE* file{std::fopen(path, "r")};
    expect(file != nullptr, "Could not open the file!");

    ix           begining{size(buffer)};
    constexpr ix CHUNK{1024};
    ix           written{0};

    do {
        reserve(buffer, CHUNK);
        written = (ix)std::fread(buffer.last, 1, CHUNK, file);
        buffer.last += written;
    } while (CHUNK == written);

    expect(std::feof(file) != 0, "Problem while reading!");
    expect(std::fclose(file) != -1, "Could not close the file!");

    return split<char>(buffer, begining).after;
}
} // namespace cthrice
