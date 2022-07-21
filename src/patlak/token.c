// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.c"
#include "prelude/string.c"

#include <stdlib.h>

/* Type of a token in patterns. */
typedef enum {
    /* Equal sign: "=". */
    CT_PATLAK_TOKEN_EQUAL,
    /* Period: ".". */
    CT_PATLAK_TOKEN_DOT,
    /* Vertical bar: "|". */
    CT_PATLAK_TOKEN_PIPE,
    /* Comma: ",". */
    CT_PATLAK_TOKEN_COMMA,
    /* Question mark: "?". */
    CT_PATLAK_TOKEN_QUESTION_MARK,
    /* Asterisk: "*". */
    CT_PATLAK_TOKEN_STAR,
    /* Plus sign: "+". */
    CT_PATLAK_TOKEN_PLUS,
    /* Opening bracket: "(". */
    CT_PATLAK_TOKEN_OPENING_BRACKET,
    /* Closing bracket: ")". */
    CT_PATLAK_TOKEN_CLOSING_BRACKET,
    /* Opening curly bracket: "{". */
    CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET,
    /* Closing curly bracket: "}". */
    CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET,
    /* Opening square bracket: "[". */
    CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET,
    /* Closing square bracket: "]". */
    CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET,
    /* Any amount of consecutive, nonseparated decimal digits without a
     * sign character. */
    CT_PATLAK_TOKEN_NUMBER,
    /* Any amount of consecutive characters inside quotation marks. */
    CT_PATLAK_TOKEN_QUOTE,
    /* Any amount of consecutive characters from the English alphabet
     * and underscores. */
    CT_PATLAK_TOKEN_IDENTIFIER
} CTPatlakTokenType;

/* Parts of a pattern string. */
typedef struct {
    /* Token type. */
    CTPatlakTokenType type;
    /* Token string. */
    CTString value;
} CTPatlakToken;

/* Dynamic array of tokens. */
typedef struct {
    /* Border before the first token. */
    CTPatlakToken* first;
    /* Border after the last token. */
    CTPatlakToken* last;
    /* Border after the last allocated token. */
    CTPatlakToken* allocated;
} CTPatlakTokenList;

/* Amount of tokens. */
CTIndex ct_patlak_tokens_list_size(CTPatlakTokenList const* tokens)
{
    return tokens->last - tokens->first;
}

/* Amount of allocated tokens. */
CTIndex ct_patlak_token_list_capacity(CTPatlakTokenList const* tokens)
{
    return tokens->allocated - tokens->first;
}

/* Amount of allocated but unused tokens. */
CTIndex ct_patlak_token_list_space(CTPatlakTokenList const* tokens)
{
    return tokens->allocated - tokens->last;
}

/* Whether there are any tokens. */
bool ct_patlak_token_list_finite(CTPatlakTokenList const* tokens)
{
    return ct_patlak_tokens_list_size(tokens) > 0;
}

/* Pointer to the token at the index. */
CTPatlakToken*
ct_patlak_token_list_get(CTPatlakTokenList const* tokens, CTIndex index)
{
    ct_expect(
        index >= 0 && index < ct_patlak_tokens_list_size(tokens),
        "Index out of bounds!");
    return tokens->first + index;
}

/* Make sure the amount of tokens will fit. Grows by at least the half of
 * the current capacity if necessary. */
void ct_patlak_token_list_reserve(CTPatlakTokenList* tokens, CTIndex amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_patlak_token_list_space(tokens);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_patlak_token_list_capacity(tokens);
    CTIndex half_capacity = capacity >> 1;
    if (growth < half_capacity) {
        growth = half_capacity;
    }

    CTIndex        new_capacity = capacity + growth;
    CTPatlakToken* memory =
        realloc(tokens->first, new_capacity * sizeof(CTPatlakToken));
    ct_expect(memory != NULL, "Could not allocate!");

    tokens->last      = memory + ct_patlak_tokens_list_size(tokens);
    tokens->first     = memory;
    tokens->allocated = memory + new_capacity;
}

/* Add to the end of tokens. */
void ct_patlak_token_list_add(CTPatlakTokenList* tokens, CTPatlakToken token)
{
    ct_patlak_token_list_reserve(tokens, 1);
    *tokens->last++ = token;
}

/* Remove the tokens. Keeps the memory. */
void ct_patlak_token_list_clear(CTPatlakTokenList* tokens)
{
    tokens->last = tokens->first;
}

/* Deallocate memory. */
void ct_patlak_token_list_free(CTPatlakTokenList* tokens)
{
    free(tokens->first);
    tokens->first     = NULL;
    tokens->last      = NULL;
    tokens->allocated = NULL;
}

/* Immutable view of tokens. */
typedef struct {
    /* Border before the first token. */
    CTPatlakToken const* first;
    /* Border after the last token. */
    CTPatlakToken const* last;
} CTPatlakTokens;

/* Amount of tokens. */
CTIndex ct_patlak_tokens_size(CTPatlakTokens const* tokens)
{
    return tokens->last - tokens->first;
}

/* Whether there are any tokens. */
bool ct_patlak_tokens_finite(CTPatlakTokens const* tokens)
{
    return ct_patlak_tokens_size(tokens) > 0;
}

/* Type of the token after the next one by the given amount. */
CTPatlakTokenType
ct_patlak_tokens_type(CTPatlakTokens const* tokens, CTIndex amount)
{
    return tokens->first[amount].type;
}

/* Type of the next token. */
CTPatlakTokenType ct_patlak_tokens_first(CTPatlakTokens const* tokens)
{
    return ct_patlak_tokens_type(tokens, 0);
}

/* Whether the next token is of the given type. */
bool ct_patlak_tokens_starts(
    CTPatlakTokens const* tokens,
    CTPatlakTokenType     type)
{
    return ct_patlak_tokens_first(tokens) == type;
}

/* Consume the next token and return its value. */
CTString ct_patlak_tokens_next(CTPatlakTokens* tokens)
{
    return tokens->first++->value;
}
