// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "patlak/builder.c"
#include "patlak/lexer.c"
#include "patlak/object.c"
#include "patlak/parser.c"
#include "patlak/printer.c"
#include "patlak/token.c"
#include "prelude/buffer.c"
#include "prelude/expect.c"
#include "prelude/file.c"
#include "prelude/split.c"
#include "prelude/string.c"

#include <stdio.h>

/* Compile the source file at the path. */
void ct_compile(char const* path)
{
    printf("Compiling %s...\n", path);
    CTBuffer buffer = {0};
    CTString file   = ct_file_load(&buffer, path);

    CTPatlakTokens tokens = {0};

    while (ct_string_finite(&file)) {
        CTSplit  split = ct_split_first(&file, '\n');
        CTString line  = split.before;
        file.first     = split.after.first + 1;

        if (ct_string_size(&line) >= 2 && ct_string_at(&line, 0) == '/' &&
            ct_string_at(&line, 1) == '/') {
            continue;
        }

        ct_patlak_tokens_clear(&tokens);
        ct_patlak_lexer(&tokens, line);
        ct_patlak_printer_tokens(&tokens);
    }

    ct_patlak_tokens_free(&tokens);
    ct_buffer_free(&buffer);
}

/* Entry to the compiler. */
int main(int argument_count, char const* const* arguments)
{
    printf("Thrice C Transpiler\n");
    printf("Running with arguments:\n");
    for (int i = 0; i < argument_count; i++) {
        printf("[%d] {%s}\n", i, arguments[i]);
    }
    printf("\n");

    ct_expect(argument_count >= 2, "Provide a thrice file!");
    for (int i = 1; i < argument_count; i++) {
        ct_compile(arguments[i]);
    }

    CTString       pattern = ct_string_terminated("hello = [+]{'Hello!'}");
    CTPatlakTokens tokens  = {0};
    CTPatlakTree   tree    = {0};

    ct_patlak_lexer(&tokens, pattern);
    ct_patlak_parser(&tree, &tokens);
    ct_patlak_printer_tree(&tree);
    ct_patlak_tree_free(&tree);
    ct_patlak_tokens_free(&tokens);

    return 0;
}
