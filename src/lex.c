// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_LEX
#define CTHR_LEX

#include "err.c"
#include "str.c"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

enum tknt {
    TKN_SEMCLN,
    TKN_OBRCKT,
    TKN_CBRCKT,
    TKN_OSBRKT,
    TKN_CSBRKT,
    TKN_OCBRKT,
    TKN_CCBRKT,
    TKN_INTEGR,
    TKN_KSZ,
    TKN_KSTR,
    TKN_KRETRN,
    TKN_ID,
    TKN_EOF
};

const char* cthr_lex_tknt_name(const enum tknt typ)
{
    switch (typ) {
        case TKN_SEMCLN:
            return "semicolon";
        case TKN_OBRCKT:
            return "opening-bracket";
        case TKN_CBRCKT:
            return "closing-bracket";
        case TKN_OSBRKT:
            return "opening-square-bracket";
        case TKN_CSBRKT:
            return "closing-square-bracket";
        case TKN_OCBRKT:
            return "opening-curly-bracket";
        case TKN_CCBRKT:
            return "closing-curly-bracket";
        case TKN_INTEGR:
            return "integer";
        case TKN_KSZ:
            return "sz keyword";
        case TKN_KSTR:
            return "str keyword";
        case TKN_KRETRN:
            return "return keyword";
        case TKN_ID:
            return "identifier";
        case TKN_EOF:
            return "end-of-file";
        default:
            return "unkown-token";
    }
}

struct tkn {
    enum tknt  typ;
    struct str val;
};

void cthr_lex_print_tkn(const struct tkn tkn)
{
    printf("%s", cthr_lex_tknt_name(tkn.typ));
    switch (tkn.typ) {
        case TKN_INTEGR:
        case TKN_ID:
            printf(" {%.*s}", (int)cthr_str_length(tkn.val), tkn.val.beg);
    }
    printf("\n");
}

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

struct lex cthr_lex_number(const struct str word, const struct str src)
{
    const size_t len = cthr_str_length(word);

    if (len == 1) {
        return cthr_lex_create(TKN_INTEGR, word, src);
    }

    bool (*digit)(uint8_t) = &cthr_str_digit;
    const uint8_t* end     = word.beg + 1;

    if ('0' == *word.beg) {
        switch (*end) {
            case 'b':
                digit = &cthr_str_digitb;
                break;
            case 'o':
                digit = &cthr_str_digito;
                break;
            case 'x':
                digit = &cthr_str_digitx;
                break;
            case '_':
                break;
            default:
                return cthr_lex_create(
                    TKN_INTEGR,
                    cthr_str_sub(word, 0, 1),
                    src);
        }

        if (++end >= word.end) {
            cthr_err("Number ended after the base indicator!");
        }
    }

    bool sep = false;
    while (end < word.end && ((*end == '_' && sep) || digit(*end))) {
        sep = *end != '_';
        end++;
    }
    if (*end == '_') {
        end--;
    }

    return cthr_lex_create(
        TKN_INTEGR,
        (struct str){.beg = word.beg, .end = end},
        src);
}

bool cthr_lex_idchr(const uint8_t chr)
{
    return chr == '_' || cthr_str_alpha(chr);
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
        return cthr_lex_number(word, src);
    }

    // Check for identifier or keyword.
    if (cthr_lex_idchr(*word.beg)) {
        const uint8_t* end = word.beg + 1;
        while (end < word.end && cthr_lex_idchr(*end) || cthr_str_digit(*end)) {
            end++;
        }
        const struct str val = {.beg = word.beg, .end = end};
#define CTHR_LEX_KEYWORD_COUNT 3
        const struct str keywords[CTHR_LEX_KEYWORD_COUNT] = {
            cthr_str_c("sz"),
            cthr_str_c("str"),
            cthr_str_c("return"),
        };
        for (size_t i = 0; i < CTHR_LEX_KEYWORD_COUNT; i++) {
            if (cthr_str_equals(keywords[i], val)) {
                return cthr_lex_create(TKN_KSZ + i, val, src);
            }
        }
        return cthr_lex_create(TKN_ID, val, src);
    }

    printf("DEBUG: Lexed word: %.*s\n", (int)cthr_str_length(word), word.beg);
    cthr_err("Unkown character!");
}

#endif // CTHR_LEX
