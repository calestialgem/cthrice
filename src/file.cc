// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "format.cc"

#include <fstream>
#include <sstream>
#include <string>

namespace cthrice::file
{
/* Process the file at the path line by line. */
template<typename StringViewConsumer>
[[nodiscard]] auto
process(char const* const path, StringViewConsumer consumer) noexcept
{
    std::ifstream file{path};
    std::string   line;

    while (!file.eof()) {
        getline(file, line);
        consumer(std::string_view{line});
        line.clear();
    }
}
} // namespace cthrice::file
