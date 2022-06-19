// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>
#include <stdlib.h>

int main(int const argc, const char* const* argv)
{
    printf("Running Thrice Transpiler to C with arguments:\n");
    for (int i = 0; i < argc; i++) {
        printf("[%d] %s\n", i, argv[i]);
    }
    return EXIT_SUCCESS;
}
