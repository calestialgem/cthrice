// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/token.c"
#include "prelude/expect.c"
#include "prelude/split.c"
#include "prelude/string.c"

/* Try to lex a punctuation mark. */
bool ct_patlak_lexer_mark(CTPatlakTokens* tokens, CTString* pattern)
{
    CTString const marks    = ct_string_terminated("=.|,?*+(){}[]");
    char const*    position = ct_string_first(&marks, ct_string_at(pattern, 0));
    if (position == marks.last) {
        return false;
    }
    CTSplit split = ct_split(pattern, 1);
    ct_patlak_tokens_add(
        tokens,
        (CTPatlakToken){
            .type  = CT_PATLAK_TOKEN_EQUAL + position - marks.first,
            .value = split.before});
    *pattern = split.after;
    return true;
}

/* Whether the character is not part of a number. */
bool ct_patlak_lexer_not_number(char character)
{
    return character < '0' || character > '9';
}

/* Try to lex a number. */
bool ct_patlak_lexer_number(CTPatlakTokens* tokens, CTString* pattern)
{
    CTSplit split = ct_split_first_fit(pattern, &ct_patlak_lexer_not_number);
    if (!ct_string_finite(&split.before)) {
        return false;
    }
    ct_patlak_tokens_add(
        tokens,
        (CTPatlakToken){.type = CT_PATLAK_TOKEN_NUMBER, .value = split.before});
    *pattern = split.after;
    return true;
}

/* Try to lex a quote. */
bool ct_patlak_lexer_quote(CTPatlakTokens* tokens, CTString* pattern)
{
    if (!ct_string_starts(pattern, '\'')) {
        return false;
    }
    char const* end = pattern->first + 1;
    while (end < pattern->last) {
        switch (*end) {
            case '\'':
                end++;
                break;
            case '\\':
                end++;
            default:
                end++;
                continue;
        }
        break;
    }
    CTSplit split = ct_split_position(pattern, end);
    ct_expect(ct_string_size(&split.before) > 2, "Quote is empty!");
    ct_expect(ct_string_finishes(&split.before, '\''), "No closing quote!");
    ct_patlak_tokens_add(
        tokens,
        (CTPatlakToken){.type = CT_PATLAK_TOKEN_QUOTE, .value = split.before});
    *pattern = split.after;
    return true;
}

/* Whether the character is not part of an identifier. */
bool ct_patlak_lexer_not_identifier(char character)
{
    return (character < 'a' || character > 'z') &&
           (character < 'A' || character > 'Z') && character != '_';
}

/* Try to lex a identifier. */
bool ct_patlak_lexer_identifier(CTPatlakTokens* tokens, CTString* pattern)
{
    CTSplit split =
        ct_split_first_fit(pattern, &ct_patlak_lexer_not_identifier);
    if (!ct_string_finite(&split.before)) {
        return false;
    }
    ct_patlak_tokens_add(
        tokens,
        (CTPatlakToken){
            .type  = CT_PATLAK_TOKEN_IDENTIFIER,
            .value = split.before});
    *pattern = split.after;
    return true;
}

/* Whether the character is not whitespace. */
bool ct_patlak_lexer_not_whitespace(char character)
{
    return character != '\t' && character != '\n' && character != ' ';
}

/* Lex away an unknown token. When unkown token appears, lex character by
 * character with -1 for unkown token type. */
void ct_patlak_lexer_unkown(CTPatlakTokens* tokens, CTString* pattern)
{
    CTSplit split = ct_split(pattern, 1);
    ct_patlak_tokens_add(
        tokens,
        (CTPatlakToken){.type = -1, .value = split.before});
    *pattern = split.after;
}

/* Lex the next token. */
void ct_patlak_lexer_next(CTPatlakTokens* tokens, CTString* pattern)
{
    // Trim the whitespace at the begining.
    pattern->first =
        ct_string_first_fit(pattern, &ct_patlak_lexer_not_whitespace);

    // If nothing is left after trimming, stop lexing.
    if (!ct_string_finite(pattern)) {
        return;
    }

    // If cannot lex any of these in the given order, the token is unknown.
    if (!ct_patlak_lexer_mark(tokens, pattern) &&
        !ct_patlak_lexer_number(tokens, pattern) &&
        !ct_patlak_lexer_quote(tokens, pattern) &&
        !ct_patlak_lexer_identifier(tokens, pattern)) {
        ct_patlak_lexer_unkown(tokens, pattern);
    }
}

/* Lex the pattern and add its tokens to the list. */
void ct_patlak_lexer(CTPatlakTokens* tokens, CTString pattern)
{
    while (ct_string_finite(&pattern)) {
        ct_patlak_lexer_next(tokens, &pattern);
    }
}
