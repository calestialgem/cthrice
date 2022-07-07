// SPDX-FileCopyrightText: 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "buffer/api.h"
#include "error/api.h"
#include "pattern/api.h"
#include "string/api.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    CHECK(argc == 2, "Provide a Thrice file!");
    struct str path = str_convert(argv[1]);
    struct bfr bfr  = {0};

    bfr = bfr_put_str(bfr, str_convert("Name of the file: "));
    bfr = bfr_put_str(bfr, path);
    bfr = bfr_put_str(bfr, str_convert("\nContents of the file:\n"));
    struct str view = bfr_view(bfr);
    printf("%.*s\n", (int)str_size(view), view.bgn);

    ptr off = bfr_size(bfr);
    bfr     = bfr_put_file(bfr, path.bgn);
    view    = bfr_view_sub(bfr, off);
    printf("%.*s\n", (int)str_size(view), view.bgn);

    bfr = bfr_destroy(bfr);

    struct ptrnctx ptrnctx = {0};
    ptrnctx = ptrn_parse(ptrnctx, str_convert("h        = 'h' 'ell' 'o'"));
    ptrnctx = ptrn_parse(ptrnctx, str_convert("a        = 'a' 'a' 'a'"));
    ptrnctx = ptrn_parse(ptrnctx, str_convert("cem      = 'Cem'"));
    ptrnctx = ptrn_parse(ptrnctx, str_convert("cam      = 'Cam'"));
    ptrnctx = ptrn_parse(ptrnctx, str_convert("escaped  = '\\''"));
    ptrnctx = ptrn_parse(ptrnctx, str_convert("other    = '\\\\\\n'"));
    ptrnctx = ptrn_parse(ptrnctx, str_convert("range    = 'a~z'"));
    ptrnctx = ptrn_destory(ptrnctx);
}
