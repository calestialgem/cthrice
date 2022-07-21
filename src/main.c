// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "patlak/context.c"
#include "patlak/printer.c"
#include "prelude/buffer.c"
#include "prelude/expect.c"
#include "prelude/file.c"
#include "prelude/split.c"
#include "prelude/string.c"

#include <stdio.h>

// /* Compile the source file at the path. */
// void ct_compile(char const* path)
// {
//     printf("Compiling %s...\n", path);
//     CTBuffer buffer = {0};
//     CTString file   = ct_file_load(&buffer, path);

// CTPatlakTokens tokens = {0};

// while (ct_string_finite(&file)) {
//     CTSplit  split = ct_split_first(&file, '\n');
//     CTString line  = split.before;
//     file.first     = split.after.first + 1;

// if (ct_string_size(&line) >= 2 && ct_string_at(&line, 0) == '/' &&
//     ct_string_at(&line, 1) == '/') {
//     continue;
// }

// ct_patlak_tokens_clear(&tokens);
// ct_patlak_lexer(&tokens, line);
// ct_patlak_printer_tokens(&tokens);
//     }

// ct_patlak_tokens_free(&tokens);
// ct_buffer_free(&buffer);
// }

/* Entry to the compiler. */
int main(int argument_count, char const* const* arguments)
{
    printf("Thrice C Transpiler\n");
    printf("Running with arguments:\n");
    for (int i = 0; i < argument_count; i++) {
        printf("[%d] {%s}\n", i, arguments[i]);
    }
    printf("\n");

    // ct_expect(argument_count >= 2, "Provide a thrice file!");
    // for (int i = 1; i < argument_count; i++) {
    //     ct_compile(arguments[i]);
    // }

#define PATTERNS_LENGTH 6
    CTString patterns[PATTERNS_LENGTH] = {
        ct_string_terminated("pata = 'pata'"),
        ct_string_terminated("pete = 'pete'"),
        ct_string_terminated("OR = 'pata' | 'pete' | 'piti'"),
        ct_string_terminated("fixed = 5'a'"),
        ct_string_terminated("ranged = [4,7]'ab'"),
        ct_string_terminated("patapete = +{pata} *{pete} ?'!'")};

    CTPatlakContext context = {0};

    for (int i = 0; i < PATTERNS_LENGTH; i++) {
        ct_patlak_compile(&context, patterns + i);
    }
    ct_patlak_printer_codes(&context.codes);

    CTString name  = ct_string_terminated("pata");
    CTString input = ct_string_terminated("pata");
    CTString match = ct_patlak_match(&context, &name, &input);
    printf("Match: %.*s\n", (int)ct_string_size(&match), match.first);

    ct_patlak_free(&context);

    return 0;
}
