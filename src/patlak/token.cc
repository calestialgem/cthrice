// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "prelude/error.cc"
#include "prelude/list.cc"
#include "prelude/types.cc"
#include "prelude/view.cc"

#include <cstdio>

namespace cthrice::patlak
{
    /* Parts of a pattern string. */
    struct Token {
        /* Type. */
        enum Type {
            /* Equal sign: "=". */
            EQUAL,
            /* Period: ".". */
            DOT,
            /* Vertical bar: "|". */
            PIPE,
            /* Comma: ",". */
            COMMA,
            /* Question mark: "?". */
            QUESTION_MARK,
            /* Asterisk: "*". */
            STAR,
            /* Plus sign: "+". */
            PLUS,
            /* Opening curly bracket: "{". */
            OPENING_CURLY_BRACKET,
            /* Closing curly bracket: "}". */
            CLOSING_CURLY_BRACKET,
            /* Opening square bracket: "[". */
            OPENING_SQUARE_BRACKET,
            /* Closing square bracket: "]". */
            CLOSING_SQUARE_BRACKET,
            /* Any amount of consecutive, nonseparated decimal digits without a
             * sign character. */
            NUMBER,
            /* Any amount of consecutive characters inside quotation marks. */
            QUOTE,
            /* Any amount of consecutive characters from the English alphabet
             * and underscores. */
            IDENTIFIER
        } typ;
        /* Token string. */
        View<const B8> val;
    };

    /* State after the last lexing operation. */
    struct Lex {
        /* The token list. */
        List<Token> tkns;
        /* Remaining pattern string. */
        View<const B8> ptrn;
        /* Whether a token was read. */
        bool lxd;
    };

    namespace token
    {
        /* Format string from the token type. */
        const B8* type_format(Token::Type typ)
        {
            switch (typ) {
                case Token::EQUAL:
                    return "= ";
                case Token::DOT:
                    return ". ";
                case Token::PIPE:
                    return "| ";
                case Token::COMMA:
                    return ", ";
                case Token::QUESTION_MARK:
                    return "? ";
                case Token::STAR:
                    return "* ";
                case Token::PLUS:
                    return "+ ";
                case Token::OPENING_CURLY_BRACKET:
                    return "{ ";
                case Token::CLOSING_CURLY_BRACKET:
                    return "} ";
                case Token::OPENING_SQUARE_BRACKET:
                    return "[ ";
                case Token::CLOSING_SQUARE_BRACKET:
                    return "] ";
                case Token::NUMBER:
                case Token::QUOTE:
                case Token::IDENTIFIER:
                    return "%.*s ";
                default:
                    return "!(%.*s) ";
            }
        }

        /* Print the token. */
        void print(Token tkn)
        {
            std::printf(
                type_format(tkn.typ),
                (int)view::size(tkn.val),
                tkn.val.bgn);
        }

        /* Print the tokens. */
        void print_all(View<const Token> tkns)
        {
            for (const Token* i = tkns.bgn; i < tkns.end; i++) {
                print(*i);
            }
            std::printf("\n");
        }

        /* Create a token with the type and the length. */
        [[nodiscard]] Lex form(Lex lex, Token::Type typ, Ix len)
        {
            return {
                .tkns = list::add(
                    lex.tkns,
                    {.typ = typ, .val = view::view_trail(lex.ptrn, len)}),
                .ptrn = view::view_tail(lex.ptrn, len),
                .lxd  = true};
        }

        /* Try to lex a punctuation mark. */
        [[nodiscard]] Lex mark(Lex lex)
        {
            View<const B8> MARKS = view::terminated("=.|,?*+{}[]");
            const B8*      pos   = view::first(MARKS, view::at(lex.ptrn, 0));
            if (pos == MARKS.end) {
                lex.lxd = false;
                return lex;
            }
            return form(lex, (Token::Type)(Token::EQUAL + pos - MARKS.bgn), 1);
        }

        /* Try to lex a number. */
        [[nodiscard]] Lex number(Lex lex)
        {
            const B8* pos = view::first_fit(lex.ptrn, [](B8 c) {
                return c < '0' || c > '9';
            });
            Ix        len = pos - lex.ptrn.bgn;
            if (len == 0) {
                lex.lxd = false;
                return lex;
            }
            return form(lex, Token::NUMBER, len);
        }

        /* Try to lex a quote. */
        [[nodiscard]] Lex quote(Lex lex)
        {
            if (view::at(lex.ptrn, 0) != '\'') {
                lex.lxd = false;
                return lex;
            }
            Ix len = 1;
            for (; len < view::size(lex.ptrn); len++) {
                switch (view::at(lex.ptrn, len)) {
                    case '\'':
                        len++;
                        break;
                    case '\\':
                        len++;
                    default:
                        continue;
                }
                break;
            }
            check(len > 2, "Quote is too short!");
            check(view::at(lex.ptrn, len - 1) == '\'', "No closing quote!");
            return form(lex, Token::QUOTE, len);
        }

        /* Try to lex an identifier. */
        [[nodiscard]] Lex identifier(Lex lex)
        {
            const B8* pos = view::first_fit(lex.ptrn, [](B8 c) {
                return (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != '_';
            });
            Ix        len = pos - lex.ptrn.bgn;
            if (len == 0) {
                lex.lxd = false;
                return lex;
            }
            return form(lex, Token::IDENTIFIER, len);
        }

        /* Lex the next token. */
        [[nodiscard]] Lex next(Lex lex)
        {
            // Trim the whitespace at the begining.
            lex.ptrn.bgn = view::first_fit(lex.ptrn, [](B8 c) {
                return c != '\t' && c != '\n' && c != ' ';
            });

            lex = mark(lex);
            if (lex.lxd) {
                return lex;
            }

            lex = number(lex);
            if (lex.lxd) {
                return lex;
            }

            lex = quote(lex);
            if (lex.lxd) {
                return lex;
            }

            lex = identifier(lex);
            if (lex.lxd) {
                return lex;
            }

            // When unkown token appears, lex character by character with -1 for
            // unkown token type.
            return form(lex, (Token::Type)-1, 1);
        }

        /* Lex the pattern and add its tokens to the list. */
        [[nodiscard]] List<Token> lex(List<Token> tkns, View<const B8> ptrn)
        {
            Lex lex = {.tkns = tkns, .ptrn = ptrn, .lxd = false};
            while (view::finite(lex.ptrn)) {
                lex = next(lex);
            }
            return lex.tkns;
        }
    } // namespace token
} // namespace cthrice::patlak
