// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/object.c"

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
        reallocarray(builder->parents.first, new_capacity, sizeof(CTIndex));
    ct_expect(memory != NULL, "Could not allocate!");

    builder->parents.last      = memory + ct_patlak_builder_size(builder);
    builder->parents.first     = memory;
    builder->parents.allocated = memory + new_capacity;
}

/* Push the last added node as the parent. */
void ct_patlak_builder_push(CTPatlakTreeBuilder* builder)
{
    ct_patlak_builder_reserve(builder, 1);
    *builder->parents.last++ = ct_patlak_tree_size(builder->tree) - 1;
}

/* Pop the last parent. */
void ct_patlak_builder_pop(CTPatlakTreeBuilder* builder)
{
    builder->parents.last--;
}

/* Add childeren to the last pushed node. */
void ct_patlak_builder_add(CTPatlakTreeBuilder* builder, CTPatlakObject object)
{
    ct_patlak_tree_add(builder->tree, object);
    ct_patlak_tree_get(builder->tree, *(builder->parents.last - 1))
        ->childeren++;
}

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
