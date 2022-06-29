// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_VIEW_HH
#define CTHRICE_VIEW_HH 1

namespace cthrice
{
    /** Immutable view of a range of data. */
    template<typename T>
    struct View {
        const T* bgn;
        const T* end;

        /** Convert a null-terminated string to a view. */
        static View<char> cstring(const char* str)
        {
            View<char> res = {.bgn = str, .end = str};
            while (*res.end != 0) {
                res.end++;
            }
            return res;
        }

        /** Find the first occurance of the given element. Returns the position
         * after the last element if the given element does not exist. */
        static const T* first_pos(View<T> view, T other)
        {
            for (const T* i = view.bgn; i < view.end; i++) {
                if (*i == other) {
                    return i;
                }
            }
            return view.end;
        }
    };
} // namespace cthrice

#endif // CTHRICE_VIEW_HH
