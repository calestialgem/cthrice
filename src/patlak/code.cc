// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/error.cc"
#include "prelude/list.cc"
#include "prelude/slice.cc"
#include "prelude/types.cc"

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
        Slice<const B8> inpt;
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
        /* Print the code. */
        void print(Code code)
        {
            switch (code.type) {
                case Code::EMPTY:
                    printf("EMPTY");
                    break;
                case Code::LITERAL:
                    printf("LITERAL {%c}", code.ltrl);
                    break;
                case Code::RANGE:
                    printf("RANGE {%c~%c}", code.bgn, code.end);
                    break;
                case Code::REFERANCE:
                    printf("REFERANCE {%05lld}", code.ref);
                    break;
                case Code::BRANCH:
                    printf("BRANCH {%lld}", code.amt);
                    break;
                case Code::TERMINAL:
                    printf("TERMINAL");
                    break;
            }
            printf(" %+lld\n", code.move);
        }

        /* Print the codes. */
        void print_all(Slice<const Code> codes)
        {
            for (const Code* i = codes.bgn; i < codes.end; i++) {
                printf("[%05lld] ", i - codes.bgn);
                print(*i);
            }
        }

        // Prototype for call before definition.
        Slice<const B8> test(Slice<const Code> codes, State init);

        /* Decode the state using the codes and add the states come after it to
         * the next states. */
        Decode decode(Slice<const Code> codes, List<State> next, State state)
        {
            debug(!state.dead, "Decoding a dead state!");
            const Code& code = slice::at(codes, state.code);

            switch (code.type) {
                case Code::EMPTY:
                    break;
                case Code::LITERAL:
                    // Check the next input and consume it.
                    state.dead = !slice::finite(state.inpt) ||
                               *state.inpt.bgn++ != code.ltrl;
                    break;
                case Code::RANGE:
                    // Check the next input and consume it.
                    state.dead = !slice::finite(state.inpt) ||
                               *state.inpt.bgn++ < code.bgn ||
                               *state.inpt.bgn > code.end;
                    break;
                case Code::REFERANCE: {
                    // Check the reffered pattern.
                    State ref = {
                        .inpt = state.inpt,
                        .code = code.ref,
                        .dead = false};
                    Slice<const B8> match = test(codes, ref);
                    // Consume the input.
                    state.inpt.bgn = match.end;
                    state.dead     = !slice::finite(match);
                } break;
                case Code::BRANCH:
                    debug(code.amt > 0, "Nonpositive branch amount!");
                    debug(
                        slice::valid(codes, state.code + code.amt),
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
                    print(code);
                    debug(false, "Unkown type!");
            }

            // Add the target state to the next states.
            state.code += code.move;
            debug(slice::valid(codes, state.code), "Movement out of bounds!");
            next = list::add(next, state);

        end:
            return {.next = next, .matched = false};
        }

        /* Decode until the end starting from the initial state. Returns the
         * initial portion of the input that was accepted by the
         * nondeterministic finite automaton first. Empty match means none of
         * the states were accepted before all states died. */
        Slice<const B8> test(Slice<const Code> codes, State init)
        {
            Slice<const B8> match{};
            List<State>     active{};
            List<State>     next{};

            // Put the initial state.
            debug(!init.dead, "Initial state is dead!");
            active = list::add(active, init);

            // Until all states die.
            while (list::finite(active)) {
                // Step all the active states and collect all the next states.
                next = list::clear(next);
                for (const State* i = active.bgn; i < active.end; i++) {
                    Decode dcde = decode(codes, next, *i);
                    next        = dcde.next;

                    // Return early if matched.
                    if (dcde.matched) {
                        match.bgn = init.inpt.bgn;
                        match.end = i->inpt.bgn;
                        debug(
                            slice::finite(match),
                            "Did not consume anything!");
                        goto end;
                    }
                }

                // Take all the nondead next states to the active states.
                active = list::clear(active);
                for (const State* i = next.bgn; i < next.end; i++) {
                    if (!i->dead) {
                        active = list::add(active, *i);
                    }
                }
            }

        end:
            active = list::free(active);
            next   = list::free(next);
            return match;
        }
    } // namespace code
} // namespace cthrice::patlak
