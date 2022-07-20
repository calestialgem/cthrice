// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/token.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

/* Type of an object in the pattern source string. */
typedef enum {
    /* Pattern definition with an equal sign. */
    CT_PATLAK_OBJECT_DEFINITION,
    /* Name of the pattern. */
    CT_PATLAK_OBJECT_DECLERATION,
    /* Parameter of a function. */
    CT_PATLAK_OBJECT_PARAMETER,
    /* Unit: Literal: reference to another pattern. */
    CT_PATLAK_OBJECT_LITERAL_REFERENCE,
    /* Unit: Literal: single character. */
    CT_PATLAK_OBJECT_LITERAL_CHARACTER,
    /* Unit: Literal: multiple characters. */
    CT_PATLAK_OBJECT_LITERAL_STRING,
    /* Unit: Literal: one of the characters in the range. */
    CT_PATLAK_OBJECT_LITERAL_RANGE,
    /* Unit: Literal: any character. */
    CT_PATLAK_OBJECT_LITERAL_WILDCARD,
    /* Unit: or binary operator. */
    CT_PATLAK_OBJECT_OR,
    /* Unit: group of units. */
    CT_PATLAK_OBJECT_GROUP,
    /* Unit: a certain number of repetition of a unit. */
    CT_PATLAK_OBJECT_REPEAT_FIXED,
    /* Unit: between some numbers of repetition of a unit. */
    CT_PATLAK_OBJECT_REPEAT_RANGE,
    /* Unit: some number or more of repetition of a unit. */
    CT_PATLAK_OBJECT_REPEAT_INFINITE,
    /* Unit: function call. */
    CT_PATLAK_OBJECT_FUNCTION_CALL,
} CTPatlakObjectType;

/* Object in the pattern source string. */
typedef struct {
    /* Type. */
    CTPatlakObjectType type;
    /* Value. */
    CTString value;
} CTPatlakObject;

/* Hierachical representation of an object in the pattern source string with its
 * subobjects. */
typedef struct {
    /* Object. */
    CTPatlakObject object;
    /* Number of its childeren. */
    CTIndex childeren;
} CTPatlakNode;

/* Tree representation of the pattern source string. */
typedef struct {
    /* Border before the first node. */
    CTPatlakNode* first;
    /* Border after the last node. */
    CTPatlakNode* last;
    /* Border after the last allocated node. */
    CTPatlakNode* allocated;
} CTPatlakTree;

/* Amount of nodes. */
CTIndex ct_patlak_tree_size(CTPatlakTree const* tree)
{
    return tree->last - tree->first;
}

/* Amount of allocated nodes. */
CTIndex ct_patlak_tree_capacity(CTPatlakTree const* tree)
{
    return tree->allocated - tree->first;
}

/* Amount of allocated but unused nodes. */
CTIndex ct_patlak_tree_space(CTPatlakTree const* tree)
{
    return tree->allocated - tree->last;
}

/* Whether there are any nodes. */
bool ct_patlak_tree_finite(CTPatlakTree const* tree)
{
    return ct_patlak_tree_size(tree) > 0;
}

/* Pointer to the node at the index. */
CTPatlakNode* ct_patlak_tree_get(CTPatlakTree const* tree, CTIndex index)
{
    ct_expect(
        index >= 0 && index < ct_patlak_tree_size(tree),
        "Index out of bounds!");
    return tree->first + index;
}

/* Make sure the amount of nodes will fit. Grows by at least the half of
 * the current capacity if necessary. */
void ct_patlak_tree_reserve(CTPatlakTree* tree, CTIndex amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_patlak_tree_space(tree);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_patlak_tree_capacity(tree);
    CTIndex half_capacity = capacity >> 1;
    if (growth < half_capacity) {
        growth = half_capacity;
    }

    CTIndex       new_capacity = capacity + growth;
    CTPatlakNode* memory =
        realloc(tree->first, new_capacity * sizeof(CTPatlakNode));
    ct_expect(memory != NULL, "Could not allocate!");

    tree->last      = memory + ct_patlak_tree_size(tree);
    tree->first     = memory;
    tree->allocated = memory + new_capacity;
}

/* Add the object to the end of tree. */
void ct_patlak_tree_add(CTPatlakTree* tree, CTPatlakObject object)
{
    ct_patlak_tree_reserve(tree, 1);
    *tree->last++ = (CTPatlakNode){.object = object, .childeren = 0};
}

/* Remove the nodes. Keeps the memory. */
void ct_patlak_tree_clear(CTPatlakTree* tree)
{
    tree->last = tree->first;
}

/* Deallocate memory. */
void ct_patlak_tree_free(CTPatlakTree* tree)
{
    free(tree->first);
    tree->first     = NULL;
    tree->last      = NULL;
    tree->allocated = NULL;
}
