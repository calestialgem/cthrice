// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_LEXER
#define CTHR_LEXER

#include "err.c"
#include "str.c"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum tknt {
    TKN_SEMCLN,
    TKN_OPARAN,
    TKN_CPARAN,
    TKN_OBRCKT,
    TKN_CBRCKT,
    TKN_OCBRKT,
    TKN_CCBRKT,
    TKN_INTEGR,
    TKN_KSZ,
    TKN_KSTR,
    TKN_KRETRN,
    TKN_ID,
    TKN_EOF
};

struct tkn {
    enum tknt  typ;
    struct str val;
};

struct lex {
    struct tkn tkn;
    struct str src;
};

struct str cthr_lex_val(const struct str src)
{
    const struct str trm = cthr_str_trim(src);
    return cthr_str_word(trm);
}

struct lex
cthr_lex_create(const enum tknt typ, const struct str val, const struct str src)
{
    return (struct lex){
        .tkn = {    .typ = typ,     .val = val},
        .src = {.beg = val.end, .end = src.end}
    };
}

bool cthr_lex_idchr(const uint8_t chr)
{
    return chr == '_' || ('a' <= chr && 'z' >= chr) ||
           ('A' <= chr && 'Z' >= chr);
}

struct lex cthr_lex(const struct str src)
{
    const struct str word = cthr_lex_val(src);

    if (!cthr_str_length(word)) {
        return cthr_lex_create(TKN_EOF, word, src);
    }

    { // Check for punctuation.
        const struct str puncs = cthr_str_c(";()[]{}");
        const uint8_t*   pos   = cthr_str_chr_first(puncs, *word.beg);

        if (pos < puncs.end) {
            return cthr_lex_create(
                TKN_SEMCLN + (pos - puncs.beg),
                cthr_str_sub(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (cthr_str_digit(*word.beg)) {
        const uint8_t* end = word.beg + 1;
        while (end < word.end && cthr_str_digit(*end)) {
            end++;
        }
        return cthr_lex_create(
            TKN_INTEGR,
            (struct str){.beg = word.beg, .end = end},
            src);
    }

    // Check for identifier.
    if (cthr_lex_idchr(*word.beg)) {
        const uint8_t* end = word.beg + 1;
        while (end < word.end &&
               (cthr_lex_idchr(*end) || cthr_str_digit(*end))) {
            end++;
        }
        return cthr_lex_create(
            TKN_INTEGR,
            (struct str){.beg = word.beg, .end = end},
            src);
    }

    printf("DEBUG: Lexed word: %.*s\n", (int)cthr_str_length(word), word.beg);
    cthr_err("Unkown character!");
}

#endif // CTHR_LEXER
