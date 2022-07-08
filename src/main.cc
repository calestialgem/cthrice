// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "prelude/error.cc"
#include "prelude/file.cc"
#include "prelude/list.cc"
#include "prelude/map.cc"
#include "prelude/types.cc"
#include "prelude/view.cc"

#include <cstdio>

using namespace cthrice;

namespace cthrice::map
{
    Hash hash(I32 age)
    {
        return age;
    }

    bool equal(I32 lhs, I32 rhs)
    {
        return lhs == rhs;
    }
} // namespace cthrice::map

int main(int argc, char** argv)
{
    check(argc >= 2, "Provide a Thrice file!");
    List<B8> bfr{};
    auto     res = file::load(bfr, argv[1]);
    bfr          = res.bfr;
    View file    = res.file;
    std::printf("%.*s\n", (int)view::size(file), file.bgn);
    bfr = list::free(bfr);

    Map<I32, View<B8>> agemap{};

    agemap = map::add(agemap, 2021 - 2000, view::terminated("Cem"));
    agemap = map::add(agemap, 2022 - 1999, view::terminated("Gülhan"));
    agemap = map::add(agemap, 2021 - 1972, view::terminated("Zülfiye"));
    agemap = map::add(agemap, 2022 - 1967, view::terminated("Hakkı"));

    View<B8>* name = map::at(agemap, 21);
    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)view::size(*name), name->bgn);
    }

    name = map::at(agemap, 23);
    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)view::size(*name), name->bgn);
    }

    name = map::at(agemap, 2021 - 1972);
    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)view::size(*name), name->bgn);
    }

    name = map::at(agemap, 2022 - 1967);
    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)view::size(*name), name->bgn);
    }

    name = map::at(agemap, 1967);
    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)view::size(*name), name->bgn);
    }

    name = map::at(agemap, -5);
    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)view::size(*name), name->bgn);
    }

    agemap = map::free(agemap);
}
