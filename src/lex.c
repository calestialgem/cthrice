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

enum tknt cthr_lex_punc(const struct str val)
{
    const struct str puncs = cthr_str_c(";()[]{}");
    const uint8_t*   pos   = cthr_str_chr_first(puncs, *val.beg);

    if (pos == puncs.end) {
        return TKN_EOF;
    }

    return TKN_SEMCLN + (pos - puncs.beg);
}

bool cthr_lex_idchr(const uint8_t chr)
{
    return chr == '_' || ('a' <= chr && 'z' >= chr) ||
           ('A' <= chr && 'Z' >= chr);
}

struct lex cthr_lex(const struct str src)
{
    const struct str val = cthr_lex_val(src);
    printf("DEBUG: LEXER: %.*s", (int)cthr_str_length(val), val.beg);
    size_t           len = cthr_str_length(val);

    if (len == 0) {
        return cthr_lex_create(TKN_EOF, val, src);
    }

    if (len == 1) {
        enum tknt punc = cthr_lex_punc(val);
        if (punc != TKN_EOF) {
            return cthr_lex_create(punc, val, src);
        }
        if (cthr_str_digit(*val.beg)) {
            return cthr_lex_create(TKN_INTEGR, val, src);
        }
        if (cthr_lex_idchr(*val.beg)) {
            return cthr_lex_create(TKN_ID, val, src);
        }
        cthr_err("Unknown token!");
    }

    if (cthr_str_digit(*val.beg)) {
        for (const uint8_t* i = val.beg + 1; i < val.end; i++) {
            if (!cthr_str_digit(*i)) {
                cthr_err("Nondigit character in a number!");
            }
        }
        return cthr_lex_create(TKN_INTEGR, val, src);
    }

    if (!cthr_lex_idchr(*val.beg)) {
        cthr_err("Invalid character at the start of an identifier!");
    }

    for (const uint8_t* i = val.beg + 1; i < val.end; i++) {
        if (!cthr_str_digit(*i) && !cthr_lex_idchr(*i)) {
            cthr_err("Invalid character in an identifier!");
        }
    }

    return cthr_lex_create(TKN_ID, val, src);
}

#endif // CTHR_LEXER
