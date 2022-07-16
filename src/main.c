// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdio.h>

/* Compile the source file at the path. */
void compile(char const* const path)
{
    printf("Compiling %s...\n", path);
}

/* Entry to the compiler. */
int main(int const argumentCount, char const* const* const arguments)
{
    printf("Thrice C Transpiler\n");
    printf("Running with arguments:\n");
    for (int i = 0; i < argumentCount; i++) {
        printf("[%d] {%s}\n", i, arguments[i]);
    }
    printf("\n");

    if (argumentCount < 2) {
        fprintf(stderr, "Provide a thrice file!");
        return -1;
    }

    for (int i = 1; i < argumentCount; i++) {
        compile(arguments[i]);
    }

    return 0;
}
