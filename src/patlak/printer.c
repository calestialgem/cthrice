// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/code.c"
#include "patlak/object.c"
#include "patlak/token.c"
#include "prelude/scalar.c"

#include <stdio.h>

/* Print the code. */
void ct_patlak_printer_code(CTPatlakCode const* code)
{
    switch (code->type) {
        case CT_PATLAK_CODE_EMPTY:
            printf("EMPTY");
            break;
        case CT_PATLAK_CODE_LITERAL:
            printf("LITERAL {%c}", code->literal);
            break;
        case CT_PATLAK_CODE_RANGE:
            printf("RANGE {%c~%c}", code->first, code->last);
            break;
        case CT_PATLAK_CODE_REFERANCE:
            printf("REFERENCE {%05ld}", code->reffered);
            break;
        case CT_PATLAK_CODE_BRANCH:
            printf("BRANCH {%ld}", code->branches);
            break;
        case CT_PATLAK_CODE_TERMINAL:
            printf("TERMINAL");
            break;
    }
    printf(" %+ld\n", code->movement);
}

/* Print the codes. */
void ct_patlak_printer_codes(CTPatlakCodes const* codes)
{
    for (CTPatlakCode const* i = codes->first; i < codes->last; i++) {
        printf("[%05ld] ", i - codes->first);
        ct_patlak_printer_code(i);
    }
}

/* Format string from the token type. */
char const* ct_patlak_printer_token_type(CTPatlakTokenType type)
{
    switch (type) {
        case CT_PATLAK_TOKEN_EQUAL:
            return "= ";
        case CT_PATLAK_TOKEN_DOT:
            return ". ";
        case CT_PATLAK_TOKEN_PIPE:
            return "| ";
        case CT_PATLAK_TOKEN_COMMA:
            return ", ";
        case CT_PATLAK_TOKEN_QUESTION_MARK:
            return "? ";
        case CT_PATLAK_TOKEN_STAR:
            return "* ";
        case CT_PATLAK_TOKEN_PLUS:
            return "+ ";
        case CT_PATLAK_TOKEN_OPENING_BRACKET:
            return "( ";
        case CT_PATLAK_TOKEN_CLOSING_BRACKET:
            return ") ";
        case CT_PATLAK_TOKEN_OPENING_CURLY_BRACKET:
            return "{ ";
        case CT_PATLAK_TOKEN_CLOSING_CURLY_BRACKET:
            return "} ";
        case CT_PATLAK_TOKEN_OPENING_SQUARE_BRACKET:
            return "[ ";
        case CT_PATLAK_TOKEN_CLOSING_SQUARE_BRACKET:
            return "] ";
        case CT_PATLAK_TOKEN_NUMBER:
        case CT_PATLAK_TOKEN_QUOTE:
        case CT_PATLAK_TOKEN_IDENTIFIER:
            return "%.*s ";
        default:
            return "!(%.*s) ";
    }
}

/* Print the token. */
void ct_patlak_printer_token(CTPatlakToken const* token)
{
    printf(
        ct_patlak_printer_token_type(token->type),
        (int)ct_string_size(&token->value),
        token->value.first);
}

/* Print the tokens. */
void ct_patlak_printer_tokens(CTPatlakTokenList const* tokens)
{
    for (CTPatlakToken const* i = tokens->first; i < tokens->last; i++) {
        ct_patlak_printer_token(i);
    }
    printf("\n");
}

/* Format string from the object type. */
char const* ct_patlak_printer_object_type(CTPatlakObjectType type)
{
    switch (type) {
        case CT_PATLAK_OBJECT_DEFINITION:
            return "pattern";
        case CT_PATLAK_OBJECT_PARAMETER:
            return "(%.*s)";
        case CT_PATLAK_OBJECT_DECLERATION:
        case CT_PATLAK_OBJECT_LITERAL_REFERENCE:
        case CT_PATLAK_OBJECT_LITERAL_CHARACTER:
        case CT_PATLAK_OBJECT_LITERAL_STRING:
        case CT_PATLAK_OBJECT_LITERAL_RANGE:
        case CT_PATLAK_OBJECT_LITERAL_WILDCARD:
        case CT_PATLAK_OBJECT_OR:
        case CT_PATLAK_OBJECT_REPEAT_FIXED:
        case CT_PATLAK_OBJECT_REPEAT_RANGE:
        case CT_PATLAK_OBJECT_REPEAT_INFINITE:
            return "%.*s";
        case CT_PATLAK_OBJECT_GROUP:
            return "{}";
        case CT_PATLAK_OBJECT_FUNCTION_CALL:
            return "%.*s()";
        default:
            return "!(%.*s)";
    }
}

/* Print the object. */
void ct_patlak_printer_object(CTPatlakObject const* object)
{
    printf(
        ct_patlak_printer_object_type(object->type),
        (int)ct_string_size(&object->value),
        object->value.first);
}

/* Print the object tree linearly. */
void ct_patlak_printer_objects(CTPatlakTree const* objects)
{
    for (CTPatlakNode const* i = objects->first; i < objects->last; i++) {
        ct_patlak_printer_object(&i->object);
        printf(" {%lu} \n", i->childeren);
    }
}

/* Print the node with the indentation. */
CTIndex ct_patlak_printer_node(CTPatlakNode* node, CTIndex indentation)
{
    for (CTIndex i = 0; i < indentation - 1; i++) {
        printf("|  ");
    }
    if (indentation > 0) {
        printf("+- ");
    }
    ct_patlak_printer_object(&node->object);
    printf("\n");
    CTIndex traversed = 1;
    for (CTIndex i = 0; i < node->childeren; i++) {
        traversed += ct_patlak_printer_node(node + traversed, indentation + 1);
    }
    return traversed;
}

/* Print the object tree. Prints the tree as a forest; with multiple roots. */
void ct_patlak_printer_tree(CTPatlakTree const* tree)
{
    CTPatlakNode* root = tree->first;
    while (root < tree->last) {
        root += ct_patlak_printer_node(root, 0);
    }
}
