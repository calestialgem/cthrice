// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.c"
#include "prelude/scalar.c"
#include "prelude/string.c"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* Pattern information. */
typedef struct {
    /* Name of the pattern. */
    CTString name;
    /* Index to the start of the pattern's code. */
    CTIndex start;
} CTPatlakPattern;

/* unsigned long long map of all the patterns. */
typedef struct {
    /* Pattern information. */
    struct {
        /* Border before the first information. */
        CTPatlakPattern* first;
        /* Border after the last information. */
        CTPatlakPattern* last;
        /* Border after the last allocated information. */
        CTPatlakPattern* allocated;
    } information;
    /* Pattern information indicies corresponding to pattern name hashes. */
    struct {
        /* Border before the first index. */
        CTIndex* first;
        /* Border before the last index. */
        CTIndex* last;
    } indicies;
} CTPatlakPatterns;

/* Range of pattern information. */
typedef struct {
    /* Border before the first information. */
    CTPatlakPattern* first;
    /* Border after the last information. */
    CTPatlakPattern* last;
} CTPatlakPatternRange;

/* Amount of pattern information in the patterns. */
CTIndex ct_patlak_patterns_information_size(CTPatlakPatterns const* patterns)
{
    return patterns->information.last - patterns->information.first;
}

/* Amount of indicies in the patterns hash map. */
CTIndex ct_patlak_patterns_indicies_size(CTPatlakPatterns const* patterns)
{
    return patterns->indicies.last - patterns->indicies.first;
}

/* Amount of pattern information in the range. */
CTIndex ct_patlak_patterns_range_size(CTPatlakPatternRange const* range)
{
    return range->last - range->first;
}

/* Pairs corresponding to the hash. */
CTPatlakPatternRange ct_patlak_patterns_range(
    CTPatlakPatterns const* patterns,
    unsigned long long      hash)
{
    CTIndex indicies = ct_patlak_patterns_indicies_size(patterns);

    // If the indicies are empty, probably the pairs are empty too.
    if (indicies == 0) {
        // Actually they are supposed to be.
        ct_expect(
            ct_patlak_patterns_information_size(patterns) == 0,
            "Indicies are empty but not the pairs!");
        return (CTPatlakPatternRange){0};
    }

    // Find the indicies that correspond to current and the next hashes.
    CTIndex* current = patterns->indicies.first + hash % indicies;
    CTIndex* next    = current + 1;

    CTPatlakPatternRange result = {
        .first = patterns->information.first + *current,
        .last  = patterns->information.last};

    // The current hash might be the last one; thus, check the next one.
    // If the next hash is valid, limit the search view from the end.
    if (next < patterns->indicies.last) {
        result.last = patterns->information.first + *next;
    }

    return result;
}

/* Find the pattern information with the name in the range. Returns null if not
 * found. */
CTPatlakPattern*
ct_patlak_patterns_find(CTPatlakPatternRange const* range, CTString const* name)
{
    for (CTPatlakPattern* i = range->first; i < range->last; i++) {
        if (ct_string_equal(&i->name, name)) {
            return i;
        }
    }
    return NULL;
}

/* Pointer to the value that corresponds to the key. Returns nullptr if
 * the value does not exist. */
CTIndex*
ct_patlak_patterns_get(CTPatlakPatterns const* patterns, CTString const* name)
{
    CTPatlakPatternRange range =
        ct_patlak_patterns_range(patterns, ct_string_hash(name));
    CTPatlakPattern* information = ct_patlak_patterns_find(&range, name);
    ct_expect(information != NULL, "Pattern does not exist!");
    return &information->start;
}

/* Maximum allowed amount of keys whose hashes give the same index after
 * modulus operation. Decreases the speed of the hashmap, but also
 * the memory usage. */
#define PATLAK_PATTERNS_MAX_COLLISION 1

/* Whether the map should be rehashed to fit the maximum collision
 * restriction. */
bool ct_patlak_patterns_need_rehash(CTPatlakPatterns const* map)
{
    CTIndex indicies    = ct_patlak_patterns_indicies_size(map);
    CTIndex information = ct_patlak_patterns_information_size(map);
    for (CTIndex i = 0; i < indicies; i++) {
        CTPatlakPatternRange range = ct_patlak_patterns_range(map, i);
        CTIndex              size  = ct_patlak_patterns_range_size(&range);
        if (size > PATLAK_PATTERNS_MAX_COLLISION) {
            return true;
        }
        information -= size;
        if (information <= PATLAK_PATTERNS_MAX_COLLISION) {
            break;
        }
    }
    return false;
}

/* Grow the indicies; double, start from 1. */
void ct_patlak_patterns_grow_indicies(CTPatlakPatterns* patterns)
{
    CTIndex size     = ct_patlak_patterns_indicies_size(patterns);
    CTIndex new_size = size << 1;
    if (new_size == 0) {
        new_size = 1;
    }
    CTIndex* memory =
        realloc(patterns->indicies.first, new_size * sizeof(CTIndex*));
    ct_expect(memory != NULL, "Could not allocate!");

    patterns->indicies.first = memory;
    patterns->indicies.last  = memory + new_size;
}

// For taking modulus of hashes when sorting.
static CTIndex ct_patlak_patterns_size;

/* Compare pattern information. */
int ct_patlak_patterns_information_compare(void const* lhs, void const* rhs)
{
    unsigned long long leftHash =
        ct_string_hash(&((CTPatlakPattern*)lhs)->name);
    unsigned long long rightHash =
        ct_string_hash(&((CTPatlakPattern*)rhs)->name);
    return (
        int)(leftHash % ct_patlak_patterns_size - rightHash % ct_patlak_patterns_size);
}

/* Recalculate indicies. */
void ct_patlak_patterns_recalculate(CTPatlakPatterns* patterns)
{
    CTIndex hash        = -1;
    CTIndex indicies    = ct_patlak_patterns_indicies_size(patterns);
    CTIndex information = ct_patlak_patterns_information_size(patterns);
    for (CTIndex i = 0; i < information; i++) {
        CTIndex current_hash =
            (CTIndex)(ct_string_hash(&patterns->information.first[i].name) % indicies);
        ct_expect(current_hash >= hash, "Pairs are not sorted correctly!");
        while (current_hash > hash) {
            patterns->indicies.first[++hash] = i;
        }
    }
    while (++hash < indicies) {
        patterns->indicies.first[hash] = information;
    }
    ct_expect(hash == indicies, "Did not correctly calculate indicies!");
}

/* Rehash until the maximum collision restriction is obeyed. */
void ct_patlak_patterns_rehash(CTPatlakPatterns* patterns)
{
    do {
        ct_patlak_patterns_grow_indicies(patterns);

        // Sort the pairs by hash.
        ct_patlak_patterns_size = ct_patlak_patterns_indicies_size(patterns);
        qsort(
            patterns->information.first,
            ct_patlak_patterns_information_size(patterns),
            sizeof(CTPatlakPattern),
            &ct_patlak_patterns_information_compare);

        ct_patlak_patterns_recalculate(patterns);
    } while (ct_patlak_patterns_need_rehash(patterns));
}

/* Amount of allocated pattern information in the patterns. */
CTIndex
ct_patlak_patterns_information_capacity(CTPatlakPatterns const* patterns)
{
    return patterns->information.allocated - patterns->information.first;
}

/* Amount of allocated but unused pattern information in the patterns. */
CTIndex ct_patlak_patterns_information_space(CTPatlakPatterns const* patterns)
{
    return patterns->information.allocated - patterns->information.last;
}

/* Make sure the amount of pattern informations will fit. Grows by at least the
 * half of the current capacity if necessary. */
void ct_patlak_patterns_information_reserve(
    CTPatlakPatterns* patterns,
    CTIndex           amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_patlak_patterns_information_space(patterns);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_patlak_patterns_information_capacity(patterns);
    CTIndex half_capacity = capacity >> 1;
    if (growth < half_capacity) {
        growth = half_capacity;
    }

    CTIndex          new_capacity = capacity + growth;
    CTPatlakPattern* memory       = realloc(
        patterns->information.first,
        new_capacity * sizeof(CTPatlakPattern));
    ct_expect(memory != NULL, "Could not allocate!");

    patterns->information.last =
        memory + ct_patlak_patterns_information_size(patterns);
    patterns->information.first     = memory;
    patterns->information.allocated = memory + new_capacity;
}

/* Put the pattern information to the position. */
void ct_patlak_patterns_information_put(
    CTPatlakPatterns* patterns,
    CTIndex           index,
    CTPatlakPattern   pattern)
{
    ct_patlak_patterns_information_reserve(patterns, 1);
    CTPatlakPattern* position = patterns->information.first + index;
    // Memmove does not work for some reason!
    for (CTPatlakPattern* i = patterns->information.last++; i > position; i--) {
        *i = *(i - 1);
    }
    *position = pattern;
}

/* Add the value to the map with the new key. If the key already exists
 * terminates. */
void ct_patlak_patterns_add(
    CTPatlakPatterns* patterns,
    CTString const*   name,
    CTIndex           start)
{
    // Get the corresponding view for the hash.
    unsigned long long   hash  = ct_string_hash(name);
    CTPatlakPatternRange range = ct_patlak_patterns_range(patterns, hash);

    // Check if it already exists.
    ct_expect(
        ct_patlak_patterns_find(&range, name) == NULL,
        "Key already exists!");

    CTPatlakPattern pattern  = {.name = *name, .start = start};
    CTIndex         indicies = ct_patlak_patterns_indicies_size(patterns);

    // If it would not create too many collisions.
    if (ct_patlak_patterns_range_size(&range) < PATLAK_PATTERNS_MAX_COLLISION &&
        indicies != 0) {
        ct_patlak_patterns_information_put(
            patterns,
            range.last - patterns->information.first,
            pattern);

        // Increase indicies.
        for (CTIndex* i = patterns->indicies.first + hash % indicies + 1;
             i < patterns->indicies.last;
             i++) {
            (*i)++;
        }
    } else {
        // Just add to the end; rehash will handle the rest.
        ct_patlak_patterns_information_put(
            patterns,
            ct_patlak_patterns_information_size(patterns),
            pattern);
        ct_patlak_patterns_rehash(patterns);
    }
}

/* Deallocate the memory. */
void ct_patlak_patterns_free(CTPatlakPatterns* patterns)
{
    free(patterns->information.first);
    patterns->information.first     = NULL;
    patterns->information.last      = NULL;
    patterns->information.allocated = NULL;
    free(patterns->indicies.first);
    patterns->indicies.first = NULL;
    patterns->indicies.last  = NULL;
}
