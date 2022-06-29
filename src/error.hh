// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_ERROR_HH
#define CTHRICE_ERROR_HH 1

#include <exception>
#include <iostream>

#define cthrice_check(cnd, msg)                                              \
    do {                                                                     \
        if (cnd) {                                                           \
            std::cout << __FILE__ << ':' << __LINE__ << ": error: " << (msg) \
                      << std::endl;                                          \
            std::terminate();                                                \
        }                                                                    \
    } while (0)

#endif // CTHRICE_ERROR_HH
