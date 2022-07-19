// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "expect.cc"

#include <cstddef>
#include <ostream>
#include <string_view>
#include <vector>

namespace cthrice::patlak
{
/* Parts of a pattern string. */
struct Token {
    /* Type of a token in patterns. */
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
    };

    /* Create by consuming the given pattern. */
    [[nodiscard]] constexpr Token(
        Type              type,
        std::string_view& pattern,
        std::size_t       length) noexcept
        : type_{type}
        , value_{pattern.substr(0, length)}
    {
        pattern.remove_prefix(length);
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

/* Print a token. */
constexpr auto operator<<(std::ostream& out, Token const& token) noexcept
    -> std::ostream&
{
    switch (token.type()) {
        case Token::EQUAL:
            out << "= ";
            break;
        case Token::DOT:
            out << ". ";
            break;
        case Token::PIPE:
            out << "| ";
            break;
        case Token::COMMA:
            out << ", ";
            break;
        case Token::QUESTION_MARK:
            out << "? ";
            break;
        case Token::STAR:
            out << "* ";
            break;
        case Token::PLUS:
            out << "+ ";
            break;
        case Token::OPENING_CURLY_BRACKET:
            out << "{ ";
            break;
        case Token::CLOSING_CURLY_BRACKET:
            out << "} ";
            break;
        case Token::OPENING_SQUARE_BRACKET:
            out << "[ ";
            break;
        case Token::CLOSING_SQUARE_BRACKET:
            out << "] ";
            break;
        case Token::NUMBER:
        case Token::QUOTE:
        case Token::IDENTIFIER:
            out << token.value();
            break;
        default:
            out << "!(" << token.value() << ')';
    }
    return out;
}

/* Print the tokens. */
constexpr auto
operator<<(std::ostream& out, std::vector<Token> const& tokens) noexcept
    -> std::ostream&
{
    for (auto const& token : tokens) {
        out << token << ' ';
    }
    return out;
}

namespace lexer
{
/* Trim the whitespace at the begining. */
constexpr auto whitespace(std::string_view& pattern) noexcept
{
    std::string_view constexpr WHITESPACE{"\t\n "};
    auto length{pattern.find_first_not_of(WHITESPACE)};
    if (length == 0) {
        return false;
    }
    pattern.remove_prefix(length);
    return true;
}

/* Try to lex a punctuation mark. */
[[nodiscard]] constexpr auto
mark(std::vector<Token>& tokens, std::string_view& pattern) noexcept
{
    std::string_view constexpr MARKS{"=.|,?*+{}[]"};
    auto mark{MARKS.find_first_of(pattern.at(0))};
    if (mark == std::string_view::npos) {
        return false;
    }
    tokens.emplace_back(
        static_cast<Token::Type>(Token::EQUAL + mark),
        pattern,
        1);
    return true;
}

/* Try to lex a number. */
[[nodiscard]] constexpr auto
number(std::vector<Token>& tokens, std::string_view& pattern) noexcept
{
    std::string_view constexpr DIGITS{"0123456789"};
    auto length{pattern.find_first_not_of(DIGITS)};
    if (length == 0) {
        return false;
    }
    tokens.emplace_back(Token::NUMBER, pattern, length);
    return true;
}

/* Try to lex a quote. */
[[nodiscard]] constexpr auto
quote(std::vector<Token>& tokens, std::string_view& pattern) noexcept
{
    if (!pattern.starts_with('\'')) {
        return false;
    }
    std::size_t length{1};
    for (std::string_view remaining{pattern.substr(1)}; !remaining.empty();) {
        auto quote{remaining.find_first_of('\'')};
        expect(quote != std::string_view::npos, "No closing quote!");
        length += quote + 1;
        if (remaining.at(quote - 1) != '\\') {
            break;
        }
        remaining.remove_prefix(quote + 1);
    }
    expect(length > 2, "Quote is empty!");
    tokens.emplace_back(Token::QUOTE, pattern, length);
    return true;
}

/* Try to lex an identifier. */
[[nodiscard]] constexpr auto
identifier(std::vector<Token>& tokens, std::string_view& pattern) noexcept
{
    std::string_view constexpr ALPHABET{
        "abcdefghijklmnopqrstuvxyzABCDEFGHİJKLMNOPQRSTUVXYZ_"};
    auto length{pattern.find_first_not_of(ALPHABET)};
    if (length == 0) {
        return false;
    }
    tokens.emplace_back(Token::NUMBER, pattern, length);
    return true;
}

/* Lex away an unknown token. When unkown token appears, lex character by
 * character with -1 for unkown token type. */
constexpr void
unknown(std::vector<Token>& tokens, std::string_view& pattern) noexcept
{
    tokens.emplace_back(static_cast<Token::Type>(-1), pattern, 1);
}

/* Lex the next token. */
constexpr void
next(std::vector<Token>& tokens, std::string_view& pattern) noexcept
{
    // If cannot lex any of these in the given order, the token is unknown.
    if (!whitespace(pattern) && !mark(tokens, pattern) &&
        !number(tokens, pattern) && !quote(tokens, pattern) &&
        !identifier(tokens, pattern)) {
        unknown(tokens, pattern);
    }
}
} // namespace lexer

/* Lex the pattern and add its tokens to the list. */
constexpr void
lex(std::vector<Token>& tokens, std::string_view pattern) noexcept
{
    while (!pattern.empty()) {
        lexer::next(tokens, pattern);
    }
}
} // namespace cthrice::patlak
