// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ERROR_H
#define ERROR_H 1

#include <stdbool.h>
#include <stddef.h>

/* Print the given message with the file and line number. */
void message(const char* file, int line, const char* type, const char* msg);

/* Check whether the condition holds or not. Aborts if the condition is false.
 * Prints the error message with the file and line number. */
void check(bool cnd, const char* file, int line, const char* msg);

/* Check the condition and abort if it is not true. Prints the error message
 * with the file and line number. */
#define CHECK(cnd, msg) check(cnd, __FILE__, __LINE__, msg)

/* Check the condition only if NDEBUG is not defined and abort if it is not
 * true. Prints the error message with the file and line number. */
#ifdef NDEBUG
#    define ASSERT(cnd, msg) ((void)0)
#else
#    define ASSERT(cnd, msg) CHECK(cnd, msg)
#endif

#endif // ERROR_H
