// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/expect.c"
#include "prelude/scalar.c"

#include <stddef.h>
#include <stdlib.h>

/* Dynamic array of characters. */
typedef struct {
    char* first;
    char* last;
    char* allocated;
} CTBuffer;

/* Amount of characters in the buffer. */
CTIndex ct_buffer_size(CTBuffer const* buffer)
{
    return buffer->last - buffer->first;
}

/* Amount of allocated characters. */
CTIndex ct_buffer_capacity(CTBuffer const* buffer)
{
    return buffer->allocated - buffer->first;
}

/* Amount of allocated but unused characters. */
CTIndex ct_buffer_space(CTBuffer const* buffer)
{
    return buffer->allocated - buffer->last;
}

/* Make sure the amount of characters will fit. Grows by at least the half of
 * the current capacity if necessary. */
void ct_buffer_reserve(CTBuffer* buffer, CTIndex amount)
{
    ct_expect(amount >= 0, "Reserving negative amount!");
    CTIndex growth = amount - ct_buffer_space(buffer);
    if (growth <= 0) {
        return;
    }

    CTIndex capacity      = ct_buffer_capacity(buffer);
    CTIndex half_capacity = capacity >> 1;
    CTIndex new_capacity =
        capacity + growth > half_capacity ? growth : half_capacity;
    char* memory = reallocarray(buffer->first, new_capacity, sizeof(char));
    ct_expect(memory != NULL, "Could not allocate!");

    buffer->last      = memory + ct_buffer_size(buffer);
    buffer->first     = memory;
    buffer->allocated = memory + new_capacity;
}

/* Deallocate memory. */
void ct_buffer_free(CTBuffer* buffer)
{
    free(buffer->first);
    buffer->first     = NULL;
    buffer->last      = NULL;
    buffer->allocated = NULL;
}
