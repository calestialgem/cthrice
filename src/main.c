// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer/api.h"
#include "error/api.h"
#include "pattern/api.h"
#include "string/api.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    CHECK(argc == 2, "Provide a file!");
    struct str path = str_convert(argv[1]);
    struct bfr bfr  = {0};
    bfr             = bfr_put_file(bfr, path.bgn);
    struct str file = bfr_view(bfr);

    struct ptrnctx ptrnctx = {0};

    while (str_finite(file)) {
        struct str line = {.bgn = file.bgn, .end = str_find(file, '\n')};
        file.bgn        = line.end + 1;
        if (str_size(line) >= 2 && *line.bgn == '/' && *(line.bgn + 1) == '/') {
            continue;
        }
        ptrnctx = ptrn_parse(ptrnctx, line);
    }

    ptrnctx = ptrn_destory(ptrnctx);
    bfr     = bfr_destroy(bfr);
}
