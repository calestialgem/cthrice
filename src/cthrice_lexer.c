// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_lexer.h"

#include "cthrice_buffer.h"
#include "cthrice_error.h"
#include "cthrice_types.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    Cthrice_Token  lst;
    Cthrice_String src;
} Lexer;

static Cthrice_String word(Cthrice_String src)
{
    Cthrice_String trm = cthrice_string_trim(src);
    return cthrice_string_first_word(trm);
}

static Lexer
create(Cthrice_Token_Type typ, Cthrice_String val, Cthrice_String src)
{
    return (Lexer){
        .lst = {    .typ = typ,     .val = val},
        .src = {.bgn = val.end, .end = src.end}
    };
}

static Lexer number(Cthrice_String word, Cthrice_String src)
{
    uptr len = cthrice_string_length(word);

    if (len == 1) {
        return create(CTHRICE_TOKEN_INTEGER, word, src);
    }

    bool (*digit)(uchr) = &cthrice_digit;
    uchr* end           = word.bgn + 1;

    if ('0' == *word.bgn) {
        switch (*end) {
            case 'b':
                digit = &cthrice_digit_bin;
                break;
            case 'o':
                digit = &cthrice_digit_oct;
                break;
            case 'x':
                digit = &cthrice_digit_hex;
                break;
            case '_':
                break;
            default:
                return create(
                    CTHRICE_TOKEN_INTEGER,
                    cthrice_string_part(word, 0, 1),
                    src);
        }

        if (++end >= word.end) {
            cthrice_error("Number ended after the base indicator!");
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

    return create(
        CTHRICE_TOKEN_INTEGER,
        (Cthrice_String){.bgn = word.bgn, .end = end},
        src);
}

static bool id_char(uchr chr)
{
    return chr == '_' || cthrice_letter(chr);
}

static Lexer next(Cthrice_String src)
{
    Cthrice_String wrd = word(src);

    if (!cthrice_string_length(wrd)) {
        return create(CTHRICE_TOKEN_END_OF_FILE, wrd, src);
    }

    { // Check for punctuation.
        Cthrice_String puncs = cthrice_string_static(";()[]{}");
        uchr*          pos   = cthrice_string_first_pos_chr(puncs, *wrd.bgn);

        if (pos < puncs.end) {
            return create(
                CTHRICE_TOKEN_SEMICOLON + (pos - puncs.bgn),
                cthrice_string_part(wrd, 0, 1),
                src);
        }
    }

    // Check for number.
    if (cthrice_digit(*wrd.bgn)) {
        return number(wrd, src);
    }

    // Check for identifier or keyword.
    if (id_char(*wrd.bgn)) {
        uchr* end = wrd.bgn + 1;
        while (end < wrd.end && (id_char(*end) || cthrice_digit(*end))) {
            end++;
        }
        Cthrice_String val = {.bgn = wrd.bgn, .end = end};

        for (uptr i = 0; i < CTHRICE_KEYWORD_COUNT; i++) {
            if (cthrice_string_equals(
                    cthrice_string_static(CTHRICE_KEYWORDS[i]),
                    val)) {
                return create(CTHRICE_TOKEN_KEYWORD_INT8 + i, val, src);
            }
        }
        return create(CTHRICE_TOKEN_IDENTIFIER, val, src);
    }

    Cthrice_String val = cthrice_string_part(wrd, 0, 1);
    return create(-1, val, src);
}

Cthrice_Lex cthrice_lex(Cthrice_String src)
{
    // Create a buffer.
    Cthrice_Buffer bfr   = {0};
    Lexer          lexer = {0};

    // Append all the tokens to the buffer.
    do {
        // Lex the next token.
        lexer = next(src);
        src   = lexer.src;

        // Append the token.
        Cthrice_String str = {
            .bgn = (uchr*)&lexer.lst,
            .end = (uchr*)&lexer.lst + sizeof(lexer.lst)};
        bfr = cthrice_buffer_append_string(bfr, str);

        // Continue until the end of the file is reached.
    } while (lexer.lst.typ != CTHRICE_TOKEN_END_OF_FILE);

    // Create an array for all the tokens.
    uptr size                   = cthrice_buffer_size(bfr);
    Cthrice_Token* restrict mem = malloc(size);

    // Copy the tokens from the buffer to the new memory.
    memcpy(mem, bfr.bgn, size);
    cthrice_buffer_destroy(bfr);

    uptr len = size / sizeof(Cthrice_Token);
    return (Cthrice_Lex){.bgn = mem, .end = mem + len};
}

Cthrice_Lex cthrice_lex_destory(Cthrice_Lex lex)
{
    free(lex.bgn);
    return (Cthrice_Lex){.bgn = 0, .end = 0};
}
