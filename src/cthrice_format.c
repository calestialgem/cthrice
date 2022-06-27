// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_format.h"

#include "cthrice_character.h"
#include "cthrice_error.h"
#include "cthrice_string.h"

#include <stdarg.h>

uchr INTRODUCTORY     = '%';
uchr WIDTH            = '*';
uchr PRECISION        = '.';
ichr FLAGS[]          = "-+ #0";
ichr SPECIFICATIONS[] = "cs";

typedef union {
#define FLAG_COUNT 5
    bool dat[FLAG_COUNT];
    struct {
        bool left;
        bool plus;
        bool spce;
        bool altn;
        bool zero;
    };
} Flags;

typedef struct {
    va_list        arp;
    Cthrice_Buffer bfr;
    Cthrice_String fmt;
    uchr*          crt;
    Flags          flg;
    unt32          wid;
    unt32          pre;
} Context;

Context consume(Context ctx)
{
    ctx.fmt.bgn = ctx.crt;
    return ctx;
}

Context skip(Context ctx)
{
    ctx.crt            = cthrice_string_first_pos_chr(ctx.fmt, INTRODUCTORY);
    Cthrice_String skp = {.bgn = ctx.fmt.bgn, .end = ctx.crt};
    ctx.bfr            = cthrice_buffer_append_string(ctx.bfr, skp);
    return ctx;
}

Context escape(Context ctx)
{
    if (*ctx.crt == INTRODUCTORY) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, INTRODUCTORY);
        ctx.crt++;
    }
    return ctx;
}

Context flags(Context ctx)
{
    Cthrice_String flags = cthrice_string_static(FLAGS);
    for (; ctx.crt < ctx.fmt.end; ctx.crt++) {
        uchr* pos = cthrice_string_first_pos_chr(flags, *ctx.crt);
        if (pos == flags.end) {
            break;
        }
        ctx.flg.dat[pos - flags.bgn] = true;
    }

    return ctx;
}

Context number(Context ctx, bool wid)
{
    unt32 num = 0;
    if (*ctx.crt == WIDTH) {
        num = va_arg(ctx.arp, unt32);
        ctx.crt++;
    } else if (cthrice_digit(*ctx.crt)) {
        do {
            ctx.crt++;
        } while (ctx.crt < ctx.fmt.end && cthrice_digit(*ctx.crt));
        num = cthrice_string_parse_u64(
            (Cthrice_String){.bgn = ctx.fmt.bgn, .end = ctx.crt});
    }
    if (wid) {
        ctx.wid = num;
    } else {
        ctx.pre = num;
    }
    return ctx;
}

Context chr(Context ctx)
{
    uchr chr = (uchr)va_arg(ctx.arp, int);
    if (ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, chr);
    }
    for (unt32 appended = 1; appended < ctx.wid; appended++) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, ' ');
    }
    if (!ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, chr);
    }

    return ctx;
}

Context string(Context ctx)
{
    Cthrice_String str = va_arg(ctx.arp, Cthrice_String);
    if (ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_string(ctx.bfr, str);
    }
    for (unt32 appended = cthrice_string_length(str); appended < ctx.wid;
         appended++) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, ' ');
    }
    if (!ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_string(ctx.bfr, str);
    }

    return ctx;
}

Context (*const FUNCTIONS[])(Context) = {&chr, &string};

Context context(Context ctx)
{
    while (cthrice_string_length(ctx.fmt)) {
        // Skip upto the introductory character.
        ctx = skip(ctx);
        if (ctx.fmt.bgn != ctx.crt) {
            ctx = consume(ctx);
            continue;
        }

        // Consume introductory character.
        ctx.crt++;
        ctx = consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error(
                "No format conversion specifier after introductory character!");
        }

        // Escape introductory character if it is repeated.
        ctx = escape(ctx);
        if (ctx.fmt.bgn != ctx.crt) {
            ctx = consume(ctx);
            continue;
        }

        // Read and consume format flags.
        ctx = flags(ctx);
        ctx = consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error("No format conversion specifier after format flags!");
        }

        // Read and consume format width.
        ctx = number(ctx, true);
        ctx = consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error("No format conversion specifier after format width!");
        }

        // Read and consume format precision.
        ctx = number(ctx, false);
        ctx = consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error(
                "No format conversion specifier after format precision!");
        }

        // Read conversion specifier.
        Cthrice_String spec = cthrice_string_static(SPECIFICATIONS);
        uchr*          pos  = cthrice_string_first_pos_chr(spec, *ctx.crt);
        if (pos == spec.end) {
            cthrice_error("Unkown format conversion specifier!");
        }

        // Format the value.
        ctx = FUNCTIONS[pos - spec.bgn](ctx);

        // Consume conversion specifier.
        ctx.crt++;
        ctx = consume(ctx);
    }
    return ctx;
}

Cthrice_Buffer cthrice_format_append(Cthrice_Buffer bfr, ichr* fmt, ...)
{
    Context ctx = {.bfr = bfr, .fmt = cthrice_string_static(fmt)};
    va_start(ctx.arp, fmt);
    ctx = context(ctx);
    va_end(ctx.arp);
    return ctx.bfr;
}

void cthrice_format_println(ichr* fmt, ...)
{
    Context ctx = {.fmt = cthrice_string_static(fmt)};
    va_start(ctx.arp, fmt);
    ctx = context(ctx);
    va_end(ctx.arp);
    cthrice_string_println(cthrice_buffer_view(ctx.bfr));
    cthrice_buffer_destroy(ctx.bfr);
}
