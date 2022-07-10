// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "code.cc"
#include "hash.cc"
#include "prelude/error.cc"
#include "prelude/list.cc"
#include "prelude/map.cc"
#include "prelude/slice.cc"
#include "prelude/types.cc"

namespace cthrice::patlak
{
    /* All pattern related data. */
    struct Context {
        /* Compiled pattern code. */
        List<Code> codes;
        /* Mapping from the pattern name to the index of its first code. */
        Map<Slice<const B8>, Ix> starts;
    };

    /* Compile the pattern by searching for references in the context. */
    [[nodiscard]] Context compile(Context ctx, Slice<const B8> ptrn);

    /* Match the pattern with the name to the input. Returns the initial portion
     * of the input that matched. Matches are checked from the begining. Empty
     * match means it did not match or the pattern was not found. */
    [[nodiscard]] Slice<const B8>
    match(Context ctx, Slice<const B8> name, Slice<const B8> inpt)
    {
        return code::test(
            list::slice(ctx.codes),
            {.inpt = inpt, .code = map::at(ctx.starts, name)});
    }

    /* Deallocate the memory. */
    [[nodiscard]] Context free(Context ctx)
    {
        ctx.codes  = list::free(ctx.codes);
        ctx.starts = map::free(ctx.starts);
        return ctx;
    }
} // namespace cthrice::patlak
