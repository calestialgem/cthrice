// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_lexer.h"

#include "cthrice_error.h"

#include <string.h>

Cthrice_String cthrice_lexer_word(Cthrice_String src)
{
    Cthrice_String trm = cthrice_string_trim(src);
    return cthrice_string_first_word(trm);
}

Cthrice_Lexer cthrice_lexer_create(
    Cthrice_Token_Type typ,
    Cthrice_String     val,
    Cthrice_String     src)
{
    return (Cthrice_Lexer){
        .lst = {    .typ = typ,     .val = val},
        .src = {.bgn = val.end, .end = src.end}
    };
}

Cthrice_Lexer cthrice_lexer_number(Cthrice_String word, Cthrice_String src)
{
    uptr len = cthrice_string_length(word);

    if (len == 1) {
        return cthrice_lexer_create(CTHRICE_TOKEN_INTEGER, word, src);
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
                return cthrice_lexer_create(
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

    return cthrice_lexer_create(
        CTHRICE_TOKEN_INTEGER,
        (Cthrice_String){.bgn = word.bgn, .end = end},
        src);
}

bool cthrice_lexer_id_char(uchr chr)
{
    return chr == '_' || cthrice_letter(chr);
}

Cthrice_Lexer cthrice_lexer_next(Cthrice_String src)
{
    Cthrice_String word = cthrice_lexer_word(src);

    if (!cthrice_string_length(word)) {
        return cthrice_lexer_create(CTHRICE_TOKEN_END_OF_FILE, word, src);
    }

    { // Check for punctuation.
        Cthrice_String puncs = cthrice_string_static(";()[]{}");
        uchr*          pos   = cthrice_string_first_pos_chr(puncs, *word.bgn);

        if (pos < puncs.end) {
            return cthrice_lexer_create(
                CTHRICE_TOKEN_SEMICOLON + (pos - puncs.bgn),
                cthrice_string_part(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (cthrice_digit(*word.bgn)) {
        return cthrice_lexer_number(word, src);
    }

    // Check for identifier or keyword.
    if (cthrice_lexer_id_char(*word.bgn)) {
        uchr* end = word.bgn + 1;
        while (end < word.end &&
               (cthrice_lexer_id_char(*end) || cthrice_digit(*end))) {
            end++;
        }
        Cthrice_String val = {.bgn = word.bgn, .end = end};

        for (uptr i = 0; i < CTHRICE_KEYWORD_COUNT; i++) {
            if (cthrice_string_equals(
                    cthrice_string_static(CTHRICE_KEYWORDS[i]),
                    val)) {
                return cthrice_lexer_create(
                    CTHRICE_TOKEN_KEYWORD_INT8 + i,
                    val,
                    src);
            }
        }
        return cthrice_lexer_create(CTHRICE_TOKEN_IDENTIFIER, val, src);
    }

    Cthrice_String val = cthrice_string_part(word, 0, 1);
    return cthrice_lexer_create(-1, val, src);
}

Cthrice_Lex cthrice_lex(Cthrice_String src)
{
    // Create a buffer.
    Cthrice_Buffer bfr   = {};
    Cthrice_Lexer  lexer = {};

    // Append all the tokens to the buffer.
    do {
        // Lex the next token.
        lexer = cthrice_lexer_next(src);
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
