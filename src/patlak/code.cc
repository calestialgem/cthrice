// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/list.cc"
#include "prelude/types.cc"
#include "prelude/view.cc"

namespace cthrice::patlak
{
    /* Compiled pattern information. These are the transitions in the
     * nondeterministic finite automaton with empty moves. */
    struct Code {
        /* Amount of code to move forward. Zero means a match. */
        Ix move;

        /* Type. */
        enum {
            /* Move regardless without consuming a character. */
            EMPTY,
            /* Move if the character equals to the literal. */
            LITERAL,
            /* Move if the character is in the range. */
            RANGE,
            /* Move if the reffered pattern matches. */
            REFERANCE,
            /* Divergence of alternative transition states. */
            BRANCH,
            /* End of a pattern, meaning a match. */
            TERMINAL
        } type;

        /* Data. */
        union {
            /* Data of LITER AL type. A single literal to match. */
            B8 ltrl;

            /* Data of RANGE type. */
            struct {
                /* Begining of the range. */
                B8 bgn;
                /* End of the range. */
                B8 end;
            };

            /* Data of REFERANCE type. Index of the reffered pattern. */
            Ix ref;

            /* Data of BRANCH type. Amount of branches. */
            Ix amt;
        };
    };

    /* State of the nondeterministic finite automaton. */
    struct State {
        /* Remaining input. */
        View<B8> inpt;
        /* Index of the code. */
        Ix code;
        /* Whether the state reached a dead end. */
        bool dead;
    };

    /* Result of decoding a state. */
    struct Decode {
        /* States that come after. */
        List<State> next;
        /* Whether the input was accepted by the automaton. */
        bool matched;
    };
} // namespace cthrice::patlak
