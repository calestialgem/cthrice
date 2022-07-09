// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/error.cc"
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

    namespace code
    {
        /* Decode the state using the codes and add the states come after it to
         * the next states. */
        Decode decode(View<Code> codes, List<State> next, State state)
        {
            debug(!state.dead, "Decoding a dead state!");
            const Code& code = view::at(codes, state.code);

            switch (code.type) {
                case Code::EMPTY:
                    break;
                case Code::LITERAL:
                    // Check the next input and consume it.
                    state.dead = !view::finite(state.inpt) ||
                               *state.inpt.bgn++ != code.ltrl;
                    break;
                case Code::RANGE:
                    // Check the next input and consume it.
                    state.dead = !view::finite(state.inpt) ||
                               *state.inpt.bgn++ < code.bgn ||
                               *state.inpt.bgn > code.end;
                    break;
                case Code::REFERANCE:
                    // Check the reffered pattern.
                    State ref = {
                        .inpt = state.inpt,
                        .code = code.ref,
                        .dead = false};
                    // TODO: Decode the pattern.
                    // TODO: Consume the input.
                    break;
                case Code::BRANCH:
                    debug(code.amt > 0, "Nonpositive branch amount!");
                    debug(
                        view::valid(codes, state.code + code.amt),
                        "Branching out of bounds!");
                    // Add all diverging states to the next states.
                    for (Ix j = 0; j < code.amt; j++) {
                        state.code++;
                        next = list::add(next, state);
                    }
                    goto end;
                case Code::TERMINAL:
                    return {.next = next, .matched = true};
                default:
                    // DEBUG: Print the unkown code.
                    // TODO: Print the code.
                    debug(false, "Unkown type!");
            }

            // Add the target state to the next states.
            state.code += code.move;
            debug(view::valid(codes, state.code), "Movement out of bounds!");
            next = list::add(next, state);

        end:
            return {.next = next, .matched = false};
        }
    } // namespace code
} // namespace cthrice::patlak
