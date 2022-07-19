// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "patlak/lexer.cc"

#include <span>
#include <string_view>

namespace cthrice::patlak
{
/* Object in the pattern source string. */
struct Object {
    /* Type of an object in the pattern source string. */
    enum Type {
        /* Pattern definition with an equal sign. */
        DEFINITION,
        /* Name of the pattern. */
        DECLERATION,
        /* Unit: Literal: reference to another pattern. */
        LITERAL_REFERENCE,
        /* Unit: Literal: single character. */
        LITERAL_CHARACTER,
        /* Unit: Literal: multiple characters. */
        LITERAL_STRING,
        /* Unit: Literal: one of the characters in the range. */
        LITERAL_RANGE,
        /* Unit: Literal: any character. */
        LITERAL_WILDCARD,
        /* Unit: and binary operator. */
        AND,
        /* Unit: or binary operator. */
        OR,
        /* Unit: group of units. */
        GROUP,
        /* Unit: a certain number of repetition of a unit. */
        REPEAT_FIXED,
        /* Unit: between some numbers of repetition of a unit. */
        REPEAT_RANGE,
        /* Unit: some number or more of repetition of a unit. */
        REPEAT_INFINITE
    };

    /* Create by consuming the total text of the given amount of tokens. */
    [[nodiscard]] constexpr Object(
        Type              type,
        std::span<Token>& tokens,
        std::size_t       amount) noexcept
        : type_{type}
        , value_{tokens[0].value().begin(), tokens[amount - 1].value().end()}
    {
        tokens = tokens.subspan(amount);
    }

    /* Type. */
    [[nodiscard]] constexpr auto type() const noexcept
    {
        return type_;
    }

    /* Portion of the pattern text that corresponds. */
    [[nodiscard]] constexpr auto value() const noexcept
    {
        return value_;
    }

private:

    Type             type_;
    std::string_view value_;
};
} // namespace cthrice::patlak
