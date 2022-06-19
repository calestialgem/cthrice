// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buf.c"

#include <stdio.h>
#include <stdlib.h>

int main(int const argc, const char* const* argv)
{
    printf("Running Thrice Transpiler to C with arguments:\n");
    for (int i = 0; i < argc; i++) {
        printf("[%d] %s\n", i, argv[i]);
    }

    const size_t CAPACITY = 1024;
    struct buf   b        = cthr_buf_create(CAPACITY);
    b                     = cthr_buf_append_file(b, "res/main.thr");
    printf("%.*s\n", (int)cthr_buf_size(b), b.beg);

    return EXIT_SUCCESS;
}
