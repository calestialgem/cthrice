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

    struct ptrns ptrns = {0};
    ptrns              = ptrn_parse(ptrns, str_convert("@h 'h' 'ell' 'o'"));
    ptrns              = ptrn_parse(ptrns, str_convert("@a 'a' 'a' 'a'"));
    ptrns              = ptrn_parse(ptrns, str_convert("@cem 'Cem'"));
    ptrns              = ptrn_parse(ptrns, str_convert("cam 'Cam'"));
    ptrns              = ptrn_parse(ptrns, str_convert("@escaped '\\''"));
    ptrns              = ptrn_parse(ptrns, str_convert("@other '\\\\\\n'"));

    CHECK(
        str_equal(str_convert("h"), ptrn_match(ptrns, str_convert("hello"))),
        "Matching failed!");
    printf("h matched!\n");

    CHECK(
        str_equal(str_convert("a"), ptrn_match(ptrns, str_convert("aaa"))),
        "Matching failed!");
    printf("a matched!\n");

    CHECK(
        str_equal(str_convert("cem"), ptrn_match(ptrns, str_convert("Cem"))),
        "Matching failed!");
    printf("cem matched!\n");

    CHECK(
        str_equal(str_convert(""), ptrn_match(ptrns, str_convert("Cam"))),
        "Not matching failed!");
    printf("cam did not match!\n");

    CHECK(
        str_equal(str_convert("escaped"), ptrn_match(ptrns, str_convert("'"))),
        "Matching failed!");
    printf("escaped matched!\n");

    CHECK(
        str_equal(str_convert("other"), ptrn_match(ptrns, str_convert("\\\n"))),
        "Matching failed!");
    printf("other matched!\n");

    ptrns = ptrn_destory(ptrns);
}
