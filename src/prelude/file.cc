// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.cc"
#include "prelude/list.cc"
#include "prelude/scalar.cc"
#include "prelude/view.cc"

#include <cstdio>

namespace cthrice
{
/* Load the contents of the file at the given path to the buffer. Returns a view
 * to the loaded contents. */
[[nodiscard]] constexpr View<char>
load(List<char>& buffer, char const* const path)
{
    std::FILE* file = std::fopen(path, "r");
    expect(file != nullptr, "Could not open the file!");

    ix begining = size(buffer);
}
} // namespace cthrice
