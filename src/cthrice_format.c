// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cthrice_format.h"

#include "cthrice_error.h"

#include <stdarg.h>

uchr CTHRICE_FORMAT_INTRODUCTORY     = '%';
uchr CTHRICE_FORMAT_WIDTH            = '*';
uchr CTHRICE_FORMAT_PRECISION        = '.';
ichr CTHRICE_FORMAT_FLAGS[]          = "-+ #0";
ichr CTHRICE_FORMAT_SPECIFICATIONS[] = "cs";

Cthrice_Format_Context (*const CTHRICE_FORMAT_FUNCTIONS[])(
    Cthrice_Format_Context) = {&cthrice_format_chr, &cthrice_format_string};

Cthrice_Format_Context cthrice_format_consume(Cthrice_Format_Context ctx)
{
    ctx.fmt.bgn = ctx.crt;
    return ctx;
}

Cthrice_Format_Context cthrice_format_skip(Cthrice_Format_Context ctx)
{
    ctx.crt =
        cthrice_string_first_pos_chr(ctx.fmt, CTHRICE_FORMAT_INTRODUCTORY);
    Cthrice_String skp = {.bgn = ctx.fmt.bgn, .end = ctx.crt};
    ctx.bfr            = cthrice_buffer_append_string(ctx.bfr, skp);
    return ctx;
}

Cthrice_Format_Context cthrice_format_escape(Cthrice_Format_Context ctx)
{
    if (*ctx.crt == CTHRICE_FORMAT_INTRODUCTORY) {
        ctx.bfr =
            cthrice_buffer_append_unt8(ctx.bfr, CTHRICE_FORMAT_INTRODUCTORY);
        ctx.crt++;
    }
    return ctx;
}

Cthrice_Format_Context cthrice_format_flags(Cthrice_Format_Context ctx)
{
    Cthrice_String flags = cthrice_string_static(CTHRICE_FORMAT_FLAGS);
    for (; ctx.crt < ctx.fmt.end; ctx.crt++) {
        uchr* pos = cthrice_string_first_pos_chr(flags, *ctx.crt);
        if (pos == flags.end) {
            break;
        }
        ctx.flg.dat[pos - flags.bgn] = true;
    }

    return ctx;
}

Cthrice_Format_Context
cthrice_format_number(Cthrice_Format_Context ctx, bool wid)
{
    unt32 num = 0;
    if (*ctx.crt == CTHRICE_FORMAT_WIDTH) {
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

Cthrice_Format_Context cthrice_format_chr(Cthrice_Format_Context ctx)
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

Cthrice_Format_Context cthrice_format_string(Cthrice_Format_Context ctx)
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

Cthrice_Format_Context cthrice_format_context(Cthrice_Format_Context ctx)
{
    while (cthrice_string_length(ctx.fmt)) {
        // Skip upto the introductory character.
        ctx = cthrice_format_skip(ctx);
        if (ctx.fmt.bgn != ctx.crt) {
            ctx = cthrice_format_consume(ctx);
            continue;
        }

        // Consume introductory character.
        ctx.crt++;
        ctx = cthrice_format_consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error(
                "No format conversion specifier after introductory character!");
        }

        // Escape introductory character if it is repeated.
        ctx = cthrice_format_escape(ctx);
        if (ctx.fmt.bgn != ctx.crt) {
            ctx = cthrice_format_consume(ctx);
            continue;
        }

        // Read and consume format flags.
        ctx = cthrice_format_flags(ctx);
        ctx = cthrice_format_consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error("No format conversion specifier after format flags!");
        }

        // Read and consume format width.
        ctx = cthrice_format_number(ctx, true);
        ctx = cthrice_format_consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error("No format conversion specifier after format width!");
        }

        // Read and consume format precision.
        ctx = cthrice_format_number(ctx, false);
        ctx = cthrice_format_consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error(
                "No format conversion specifier after format precision!");
        }

        // Read conversion specifier.
        Cthrice_String spec =
            cthrice_string_static(CTHRICE_FORMAT_SPECIFICATIONS);
        uchr* pos = cthrice_string_first_pos_chr(spec, *ctx.crt);
        if (pos == spec.end) {
            cthrice_error("Unkown format conversion specifier!");
        }

        // Format the value.
        ctx = CTHRICE_FORMAT_FUNCTIONS[pos - spec.bgn](ctx);

        // Consume conversion specifier.
        ctx.crt++;
        ctx = cthrice_format_consume(ctx);
    }
    return ctx;
}

Cthrice_Buffer cthrice_format_append(Cthrice_Buffer bfr, ichr* fmt, ...)
{
    Cthrice_Format_Context ctx = {
        .bfr = bfr,
        .fmt = cthrice_string_static(fmt)};
    va_start(ctx.arp, fmt);
    ctx = cthrice_format_context(ctx);
    va_end(ctx.arp);
    return ctx.bfr;
}

void cthrice_format_println(ichr* fmt, ...)
{
    Cthrice_Format_Context ctx = {.fmt = cthrice_string_static(fmt)};
    va_start(ctx.arp, fmt);
    ctx = cthrice_format_context(ctx);
    va_end(ctx.arp);
    cthrice_string_println(cthrice_buffer_view(ctx.bfr));
    cthrice_buffer_destroy(ctx.bfr);
}
