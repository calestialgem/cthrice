// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHRICE_FORMAT
#define CTHRICE_FORMAT

#include "buffer.c"
#include "error.c"
#include "scalar.c"
#include "string.c"

#include <stdarg.h>
#include <stdio.h>

uchr CTHRICE_FORMAT_INTRODUCTORY     = '%';
uchr CTHRICE_FORMAT_WIDTH            = '*';
uchr CTHRICE_FORMAT_PRECISION        = '.';
ichr CTHRICE_FORMAT_FLAGS[]          = "-+ #0";
ichr CTHRICE_FORMAT_SPECIFICATIONS[] = "csdioxXufFeEaAgGnP";

typedef union {
#define CTHRICE_FORMAT_FLAG_COUNT 5
    bool dat[CTHRICE_FORMAT_FLAG_COUNT];
    struct {
        bool left;
        bool plus;
        bool spce;
        bool altn;
        bool zero;
    };
} Cthrice_Format_Flags;

typedef struct {
    va_list              arp;
    Cthrice_Buffer       bfr;
    Cthrice_String       fmt;
    uchr*                crt;
    Cthrice_Format_Flags flg;
    unt32                wid;
    unt32                pre;
    uptr                 mod;
} Cthrice_Format_Context;

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

Cthrice_Format_Context cthrice_format_modifier(Cthrice_Format_Context ctx)
{
    Cthrice_String specifiers =
        cthrice_string_static(CTHRICE_FORMAT_SPECIFICATIONS);
    uchr* pos = cthrice_string_first_pos_chr(specifiers, *ctx.crt);

    while (ctx.crt < ctx.fmt.end && pos == specifiers.end) {
        pos = cthrice_string_first_pos_chr(specifiers, *ctx.crt);
        ctx.crt++;
    }

    Cthrice_String mod = {.bgn = ctx.fmt.bgn, .end = ctx.crt};

#define CTHRICE_FORMAT_MOD_COUNT 9
    Cthrice_String mods[CTHRICE_FORMAT_MOD_COUNT] = {
        cthrice_string_static(""),
        cthrice_string_static("hh"),
        cthrice_string_static("h"),
        cthrice_string_static("l"),
        cthrice_string_static("ll"),
        cthrice_string_static("j"),
        cthrice_string_static("z"),
        cthrice_string_static("t"),
        cthrice_string_static("L")};
    ctx.mod = -1;
    for (uptr i = 0; i < CTHRICE_FORMAT_MOD_COUNT; i++) {
        if (cthrice_string_equals(mods[i], mod)) {
            ctx.mod = i;
            break;
        }
    }
    if (ctx.mod == -1) {
        cthrice_error("Unkown length modifier!");
    }

    return ctx;
}

Cthrice_Format_Context cthrice_format_chr(Cthrice_Format_Context ctx)
{
    if (ctx.mod != 0) {
        cthrice_error("Unsupported length modifier for formatting a char!");
    }
    uchr chr = (uchr)va_arg(ctx.arp, int);
    if (ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, chr);
    }
    unt32 wid = ctx.wid;
    while (wid--) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, ' ');
    }
    if (!ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_unt8(ctx.bfr, chr);
    }

    return ctx;
}

Cthrice_Format_Context cthrice_format_string(Cthrice_Format_Context ctx)
{
    if (ctx.mod != 0) {
        cthrice_error("Unsupported length modifier for formatting a string!");
    }
    Cthrice_String str = va_arg(ctx.arp, Cthrice_String);
    if (ctx.flg.left) {
        ctx.bfr = cthrice_buffer_append_string(ctx.bfr, str);
    }
    unt32 wid = ctx.wid;
    while (wid--) {
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

        // Read and consume length modifier.
        ctx = cthrice_format_modifier(ctx);
        ctx = cthrice_format_consume(ctx);
        if (!cthrice_string_length(ctx.fmt)) {
            cthrice_error(
                "No format conversion specifier after length modifiers!");
        }

        // Read conversion specifier and format value.
        switch (*ctx.crt) {
            case 'c':
                ctx = cthrice_format_chr(ctx);
                break;
            case 's':
                ctx = cthrice_format_string(ctx);
                break;
            case 'd':
            case 'i':
                // ctx = cthrice_format_signed(ctx);
                // break;
            case 'o':
                // ctx = cthrice_format_oct(ctx);
                // break;
            case 'x':
            case 'X':
                // ctx = cthrice_format_hex(ctx);
                // break;
            case 'u':
                // ctx = cthrice_format_unsigned(ctx);
                // break;
            case 'f':
            case 'F':
                // ctx = cthrice_format_float(ctx);
                // break;
            case 'e':
            case 'E':
                // ctx = cthrice_format_float_e(ctx);
                // break;
            case 'a':
            case 'A':
                // ctx = cthrice_format_float_a(ctx);
                // break;
            case 'g':
            case 'G':
                // ctx = cthrice_format_float_g(ctx);
                // break;
            case 'n':
                // ctx = cthrice_format_count(ctx);
                // break;
            case 'p':
                // ctx = cthrice_format_pointer(ctx);
                // break;
                cthrice_error("Unsupported format conversion specifier!");
                break;
            default:
                cthrice_error("Unkown format conversion specifier!");
        }

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

#endif // CTHRICE_FORMAT
