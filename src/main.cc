// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "error.hh"

int main(int argc, char** argv)
{
    cthrice_check(argc < 2, "Provide a Thrice file!");
    std::cout << "File Name: " << argv[1] << std::endl;
}
