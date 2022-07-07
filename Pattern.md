<!-- SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com> -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Pattern Language

## Definition

`<identifier> = <pattern>`

### Identifier

Pattern identifiers can have letters from the English alphabet and underscores.
The length is unbounded.

## Whitespace

Whitespace in the pattern is neglected but it is used to separate identifiers
from each other.

## Literals

| Syntax                    | Name                    |
| ------------------------- | ----------------------- |
| `<identifier>`            | reference literal       |
| `'<character>'`           | character literal       |
| `'<string>'`              | string literal          |
| `'<character~character>'` | character range literal |
| `.`                       | wildcard literal        |

### Reference

Matching another pattern in the set.

### Character

Matching a character. Which can be escaped using `\`.

| Escape Sequence                   | Name                                                       |
| --------------------------------- | ---------------------------------------------------------- |
| `'\a'`                            | alert                                                      |
| `'\b'`                            | backspace                                                  |
| `'\f'`                            | formfeed page break                                        |
| `'\n'`                            | newline                                                    |
| `'\r'`                            | carriage return                                            |
| `'\t'`                            | horizontal tab                                             |
| `'\v'`                            | vertical tab                                               |
| `'\<up-hex-digit><up-hex-digit>'` | character with the given uppercase hexadecimal ASCII value |
| `'\\'`                            | backslash                                                  |
| `'\''`                            | apostroph                                                  |
| `'\~'`                            | tilde                                                      |

### String

More than one characters. Exactly maches to the same characters ordered the same
way.

### Character Range

Two character, which can be escaped or not, separated with a `~` sign. Matches a
single character, whose ASCII value is bigger than the character on the left and
smaller than the character on the right, or the characters themselves (inclusive
range).

### Wildcard

Matches any character, like an unbounded character range: `'\00~\FF'`.

## Units

| Unit                        | Name            |
| --------------------------- | --------------- |
| `<literal>`                 | literal         |
| `<unit> <unit>`             | and             |
| `<unit> \| <unit>`          | or              |
| `{<unit>}`                  | group           |
| `[<number>]<unit>`          | fixed repeat    |
| `[<number>,<number>]<unit>` | range repeat    |
| `[<number>,]<unit>`         | infinite repeat |

### Literal

All literals are also units.

### And

First matches the unit on the left hand side, then matches the unit on the right
hand side.

### Or

Matches the unit on the left hand side or right hand side.

### Group

Units are taken from left to right. Group forces the pattern inside to be a
single unit. Meaning it can be used to take units right to left if wanted.

### Fixed Repeat

Same as repeating the unit as the given number of times. The number must be
positive.

### Range Repeat

Same as repeating the unit from the number given on the left upto the number
given on the right times. The numbers must be nonnegative. The upper bound must
be bigger than the lower bound.

### Infinite Repeat

Same as range repeat, but when the upper bound is ommited, it is taken as
positive infinity.

## Speacial Repeat

| Syntax              | Equivalent Syntax    | Name                |
| ------------------- | -------------------- | ------------------- |
| `[,<number>]<unit>` | `[0,<number>]<unit>` | omitted lower bound |
| `[?]<unit>`         | `[0,1]<unit>`        | optional            |
| `[*]<unit>`         | `[0,]<unit>`         | zero or more        |
| `[+]<unit>`         | `[1,]<unit>`         | one or more         |

## Omitted Square Brackets

| Syntax            | Equivalent Syntax  | Name         |
| ----------------- | ------------------ | ------------ |
| `<number> <unit>` | `[<number>]<unit>` | fixed repeat |
| `? <unit>`        | `[?]<unit>`        | optional     |
| `* <unit>`        | `[*]<unit>`        | zero or more |
| `+ <unit>`        | `[+]<unit>`        | one or more  |
