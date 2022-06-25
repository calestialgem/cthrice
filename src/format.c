// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_FORMAT
#define THRICE_FORMAT

#include "buffer.c"
#include "error.c"
#include "string.c"

#include <stdarg.h>
#include <stdint.h>

const uint8_t     THRICE_FORMAT_INTRODUCTORY = '%';
const uint8_t     THRICE_FORMAT_WIDTH        = '*';
const uint8_t     THRICE_FORMAT_PRECISION    = '.';
const char* const THRICE_FORMAT_FLAGS        = "-+ #0";
#define THRICE_FORMAT_FLAG_COUNT 5
const char* const THRICE_FORMAT_SPECIFICATIONS = "csdioxXufFeEaAgGnP";

typedef union {
    bool dat[THRICE_FORMAT_FLAG_COUNT];
    struct {
        bool left;
        bool plus;
        bool spce;
        bool altn;
        bool zero;
    };
} ThriceFormatFlags;

typedef struct {
    va_list           arp;
    Cthrice_Buffer    buf;
    Cthrice_String    fmt;
    const uint8_t*    crt;
    ThriceFormatFlags flg;
    uint32_t          wid;
    uint32_t          pre;
    size_t            mod;
} ThriceFormatContext;

ThriceFormatContext thriceFormatConsume(ThriceFormatContext ctx)
{
    ctx.fmt.bgn = ctx.crt;
    return ctx;
}

ThriceFormatContext thriceFormatSkip(ThriceFormatContext ctx)
{
    ctx.crt = cthrice_string_first_pos_chr(ctx.fmt, THRICE_FORMAT_INTRODUCTORY);
    ctx.buf = cthrice_buffer_append_string(
        ctx.buf,
        (Cthrice_String){.bgn = ctx.fmt.bgn, .end = ctx.crt});
    return ctx;
}

ThriceFormatContext thriceFormatEscape(ThriceFormatContext ctx)
{
    if (cthrice_string_length(ctx.fmt) < 2) {
        cthrice_error("No format conversion specifier!");
    }
    ctx.crt++;
    ctx = thriceFormatConsume(ctx);
    if (*ctx.crt == THRICE_FORMAT_INTRODUCTORY) {
        ctx.buf = cthrice_buffer_append_u8(ctx.buf, THRICE_FORMAT_INTRODUCTORY);
        ctx.crt++;
    }
    return ctx;
}

ThriceFormatContext thriceFormatFlags(ThriceFormatContext ctx)
{
    const Cthrice_String flags = cthrice_string_static(THRICE_FORMAT_FLAGS);
    for (; ctx.crt < ctx.fmt.end; ctx.crt++) {
        const uint8_t* pos = cthrice_string_first_pos_chr(flags, *ctx.crt);
        if (pos == flags.end) {
            break;
        }
        ctx.flg.dat[pos - flags.bgn] = true;
    }

    if (ctx.crt == ctx.fmt.end) {
        cthrice_error("No format conversion specifier!");
    }

    return ctx;
}

ThriceFormatContext thriceFormatNumber(ThriceFormatContext ctx, bool const wid)
{
    uint32_t num = 0;
    if (*ctx.crt == THRICE_FORMAT_WIDTH) {
        num = va_arg(ctx.arp, uint32_t);
        ctx.crt++;
    } else if (cthrice_digit(*ctx.crt)) {
        do {
            ctx.crt++;
        } while (ctx.crt < ctx.fmt.end && cthrice_digit(*ctx.crt));
        num = cthrice_string_parse_u64(
            (Cthrice_String){.bgn = ctx.fmt.bgn, .end = ctx.crt});
    }
    if (ctx.crt == ctx.fmt.end) {
        cthrice_error("No format conversion specifier!");
    }

    if (wid) {
        ctx.wid = num;
    } else {
        ctx.pre = num;
    }
    return ctx;
}

ThriceFormatContext thriceFormatModifier(ThriceFormatContext ctx)
{
    const Cthrice_String specifiers =
        cthrice_string_static(THRICE_FORMAT_SPECIFICATIONS);
    const uint8_t* pos = cthrice_string_first_pos_chr(specifiers, *ctx.crt);

    while (ctx.crt < ctx.fmt.end && pos < specifiers.end) {
        pos = cthrice_string_first_pos_chr(specifiers, *ctx.crt);
        ctx.crt++;
    }

    const Cthrice_String mod = {.bgn = ctx.fmt.bgn, .end = ctx.crt};

    if (ctx.crt == ctx.fmt.end) {
        cthrice_error("No format conversion specifier!");
    }

#define THRICE_FORMAT_MOD_COUNT 9
    const Cthrice_String mods[THRICE_FORMAT_MOD_COUNT] = {
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
    for (size_t i = 0; i < THRICE_FORMAT_MOD_COUNT; i++) {
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

ThriceFormatContext thriceFormatChr(ThriceFormatContext ctx)
{
    if (ctx.mod != 0) {
        cthrice_error("Unsupported length modifier for formatting a char!");
    }
    const uint8_t chr = (uint8_t)va_arg(ctx.arp, int);
    if (ctx.flg.left) {
        ctx.buf = cthrice_buffer_append_u8(ctx.buf, chr);
    }
    uint32_t wid = ctx.wid;
    while (--wid) {
        ctx.buf = cthrice_buffer_append_u8(ctx.buf, ' ');
    }
    if (!ctx.flg.left) {
        ctx.buf = cthrice_buffer_append_u8(ctx.buf, chr);
    }

    return ctx;
}

ThriceFormatContext thriceFormatString(ThriceFormatContext ctx)
{
    if (ctx.mod != 0) {
        cthrice_error("Unsupported length modifier for formatting a string!");
    }
    const Cthrice_String str = va_arg(ctx.arp, Cthrice_String);
    if (ctx.flg.left) {
        ctx.buf = cthrice_buffer_append_string(ctx.buf, str);
    }
    uint32_t wid = ctx.wid;
    while (--wid) {
        ctx.buf = cthrice_buffer_append_u8(ctx.buf, ' ');
    }
    if (!ctx.flg.left) {
        ctx.buf = cthrice_buffer_append_string(ctx.buf, str);
    }

    return ctx;
}

Cthrice_Buffer
thriceFormatAppend(Cthrice_Buffer buf000, Cthrice_String fmt000, ...)
{
    ThriceFormatContext ctx = {.buf = buf000, .fmt = fmt000};
    va_start(ctx.arp, fmt000);

    while (cthrice_string_length(ctx.fmt)) {
        ctx = thriceFormatSkip(ctx);
        if (ctx.fmt.bgn != ctx.crt) {
            ctx = thriceFormatConsume(ctx);
            continue;
        }

        ctx = thriceFormatEscape(ctx);
        if (ctx.fmt.bgn != ctx.crt) {
            ctx = thriceFormatConsume(ctx);
            continue;
        }

        ctx = thriceFormatFlags(ctx);
        ctx = thriceFormatConsume(ctx);

        ctx = thriceFormatNumber(ctx, true);
        ctx = thriceFormatConsume(ctx);

        ctx = thriceFormatNumber(ctx, false);
        ctx = thriceFormatConsume(ctx);

        ctx = thriceFormatModifier(ctx);
        ctx = thriceFormatConsume(ctx);

        switch (*ctx.crt) {
            case 'c':
                ctx = thriceFormatChr(ctx);
                break;
            case 's':
                ctx = thriceFormatString(ctx);
                break;
            case 'd':
            case 'i':
                // ctx = thriceFormatSigned(ctx);
                // break;
            case 'o':
                // ctx = thriceFormatOct(ctx);
                // break;
            case 'x':
            case 'X':
                // ctx = thriceFormatHex(ctx);
                // break;
            case 'u':
                // ctx = thriceFormatUnsigned(ctx);
                // break;
            case 'f':
            case 'F':
                // ctx = thriceFormatFloat(ctx);
                // break;
            case 'e':
            case 'E':
                // ctx = thriceFormatFloatE(ctx);
                // break;
            case 'a':
            case 'A':
                // ctx = thriceFormatFloatA(ctx);
                // break;
            case 'g':
            case 'G':
                // ctx = thriceFormatFloatG(ctx);
                // break;
            case 'n':
                // ctx = thriceFormatCount(ctx);
                // break;
            case 'p':
                // ctx = thriceFormatPointer(ctx);
                // break;
                cthrice_error("Unsupported format conversion specifier!");
                break;
            default:
                cthrice_error("Unkown format conversion specifier!");
        }
    }

    va_end(ctx.arp);
    return ctx.buf;
}

#endif // THRICE_FORMAT
