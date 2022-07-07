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

    printf("%.*s\n", (int)str_size(file), file.bgn);

    ptrnctx = ptrn_destory(ptrnctx);
    bfr     = bfr_destroy(bfr);
}
