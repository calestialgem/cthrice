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

int main(int argc, char** argv)
{
    check(argc >= 2, "Provide a Thrice file!");
    List<b8> bfr{};
    auto     res = file::load(bfr, argv[1]);
    bfr          = res.bfr;
    View file    = res.file;
    std::printf("%.*s\n", (int)View<b8>::size(file), file.bgn);
    bfr = List<b8>::free(bfr);

    struct Age {
        int v;

        static hash hash(Age age)
        {
            return age.v;
        }

        static bool equal(Age lhs, Age rhs)
        {
            return lhs.v == rhs.v;
        }
    };

    Map<Age, View<b8>> agemap{};

    agemap = Map<Age, View<b8>>::add(
        agemap,
        Age{2021 - 2000},
        View<b8>::terminated("Cem"));
    agemap = Map<Age, View<b8>>::add(
        agemap,
        Age{2022 - 1999},
        View<b8>::terminated("Gülhan"));
    agemap = Map<Age, View<b8>>::add(
        agemap,
        Age{2021 - 1972},
        View<b8>::terminated("Zülfiye"));
    agemap = Map<Age, View<b8>>::add(
        agemap,
        Age{2022 - 1967},
        View<b8>::terminated("Hakkı"));

    View<b8>* name = Map<Age, View<b8>>::at(agemap, Age{21});

    if (name != nullptr) {
        std::printf("Name: {%.*s}\n", (int)View<b8>::size(*name), name->bgn);
    }

    agemap = Map<Age, View<b8>>::free(agemap);
}
