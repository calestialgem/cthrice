// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/object.c"
#include "prelude/expect.c"

#include <stdio.h>

/* Tree builder. */
typedef struct {
    /* Tree that is being built. */
    CTPatlakTree* tree;
    /* Indicies of the parents in the tree. */
    struct {
        /* Border before the first index. */
        CTIndex* first;
        /* Border after the last index. */
        CTIndex* last;
        /* Border after the last allocated index. */
        CTIndex* allocated;
    } parents;
} CTPatlakTreeBuilder;

/* Amount of parents. */
CTIndex ct_patlak_builder_size(CTPatlakTreeBuilder const* builder)
{
    return builder->parents.last - builder->parents.first;
}

/* Amount of allocated parents. */
CTIndex ct_patlak_builder_capacity(CTPatlakTreeBuilder const* builder)
{
    return builder->parents.allocated - builder->parents.first;
}

/* Amount of allocated but unused parents. */
CTIndex ct_patlak_builder_space(CTPatlakTreeBuilder const* builder)
{
    return builder->parents.allocated - builder->parents.last;
}

/* Whether there are any parents. */
bool ct_patlak_builder_finite(CTPatlakTreeBuilder const* builder)
{
    return ct_patlak_builder_size(builder) > 0;
}

/* Pointer to the parent at the index. */
CTIndex*
ct_patlak_builder_get(CTPatlakTreeBuilder const* builder, CTIndex index)
{
    ct_expect(
        index >= 0 && index < ct_patlak_builder_size(builder),
        "Index out of bounds!");
    return builder->parents.first + index;
}

/* Make sure the amount of parents will fit. Grows by at least the half of
 * the current capacity if necessary. */
void ct_patlak_builder_reserve(CTPatlakTreeBuilder* builder, CTIndex amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_patlak_builder_space(builder);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_patlak_builder_capacity(builder);
    CTIndex half_capacity = capacity >> 1;
    if (growth < half_capacity) {
        growth = half_capacity;
    }

    CTIndex  new_capacity = capacity + growth;
    CTIndex* memory =
        realloc(builder->parents.first, new_capacity * sizeof(CTIndex));
    ct_expect(memory != NULL, "Could not allocate!");

    builder->parents.last      = memory + ct_patlak_builder_size(builder);
    builder->parents.first     = memory;
    builder->parents.allocated = memory + new_capacity;
}

/* Push the last added node as the parent. */
void ct_patlak_builder_push(CTPatlakTreeBuilder* builder)
{
    ct_expect(ct_patlak_tree_finite(builder->tree), "There is no parent node!");
    ct_patlak_builder_reserve(builder, 1);
    *builder->parents.last++ = ct_patlak_tree_size(builder->tree) - 1;
}

/* Pop the last parent. */
void ct_patlak_builder_pop(CTPatlakTreeBuilder* builder)
{
    ct_expect(ct_patlak_builder_finite(builder), "There is no parent node!");
    builder->parents.last--;
}

/* Get the last pushed node. */
CTPatlakNode* ct_patlak_builder_last(CTPatlakTreeBuilder const* builder)
{
    if (ct_patlak_builder_finite(builder)) {
        return ct_patlak_tree_get(builder->tree, *(builder->parents.last - 1));
    }
    return NULL;
}

/* Add childeren to the last pushed node. */
void ct_patlak_builder_add(CTPatlakTreeBuilder* builder, CTPatlakObject object)
{
    ct_patlak_tree_add(builder->tree, object);
    CTPatlakNode* last = ct_patlak_builder_last(builder);
    if (last != NULL) {
        last->childeren++;
    }
}

/* Add a root to the end of the tree, turning it into a forest. Directly pushes
 * the new root. */
void ct_patlak_builder_add_root(
    CTPatlakTreeBuilder* builder,
    CTPatlakObject       object)
{
    ct_patlak_tree_add(builder->tree, object);
    ct_patlak_builder_push(builder);
}

// /* Add the subtree as the childeren of the last pushed node. */
// void ct_patlak_builder_add_subtree(
//     CTPatlakTreeBuilder* builder,
//     CTPatlakNode const*  subtree)
// {
//     ct_patlak_builder_add(builder, subtree->object);
//     ct_patlak_builder_push(builder);
//     CTPatlakNode const* child = subtree;
//     for (CTIndex i = 0; i < subtree->childeren; i++) {
//         child++;
//         ct_patlak_builder_add_subtree(builder, child);
//         child += child->childeren;
//     }
//     ct_patlak_builder_pop(builder);
// }

// /* Move the last pushed subtree from the soure to the destination. */
// void ct_patlak_builder_move_subtree(
//     CTPatlakTreeBuilder* destination,
//     CTPatlakTreeBuilder* source)
// {
//     CTIndex moved = *(source->parents.last - 1);
//     ct_patlak_builder_add_subtree(
//         destination,
//         ct_patlak_tree_get(source->tree, moved));
//     ct_patlak_builder_pop(source);
//     source->tree->last = source->tree->first + moved;

// for (CTIndex i = moved - 1; i >= 0; i--) {
//     CTPatlakNode* candidate = source->tree->first + i;
//     if (candidate->childeren == moved - i) {
//         candidate->childeren--;
//     }
// }
// }

/* Remove the parents. Keeps the memory. */
void ct_patlak_builder_clear(CTPatlakTreeBuilder* builder)
{
    builder->parents.last = builder->parents.first;
}

/* Deallocate memory. */
void ct_patlak_builder_free(CTPatlakTreeBuilder* builder)
{
    free(builder->parents.first);
    builder->parents.first     = NULL;
    builder->parents.last      = NULL;
    builder->parents.allocated = NULL;
}
