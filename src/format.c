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
    ThriceBuffer      buf;
    ThriceString      fmt;
    const uint8_t*    crt;
    ThriceFormatFlags flg;
    uint32_t          wid;
    uint32_t          pre;
    size_t            mod;
} ThriceFormatContext;

ThriceFormatContext thriceFormatConsume(ThriceFormatContext ctx)
{
    ctx.fmt.beg = ctx.crt;
    return ctx;
}

ThriceFormatContext thriceFormatSkip(ThriceFormatContext ctx)
{
    ctx.crt = thriceStringFirstPosChr(ctx.fmt, THRICE_FORMAT_INTRODUCTORY);
    ctx.buf = thriceBufferAppendString(
        ctx.buf,
        (ThriceString){.beg = ctx.fmt.beg, .end = ctx.crt});
    return ctx;
}

ThriceFormatContext thriceFormatEscape(ThriceFormatContext ctx)
{
    if (thriceStringLength(ctx.fmt) < 2) {
        thriceError("No format conversion specifier!");
    }
    ctx.crt++;
    ctx = thriceFormatConsume(ctx);
    if (*ctx.crt == THRICE_FORMAT_INTRODUCTORY) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, THRICE_FORMAT_INTRODUCTORY);
        ctx.crt++;
    }
    return ctx;
}

ThriceFormatContext thriceFormatFlags(ThriceFormatContext ctx)
{
    const ThriceString flags = thriceStringStatic(THRICE_FORMAT_FLAGS);
    for (; ctx.crt < ctx.fmt.end; ctx.crt++) {
        const uint8_t* pos = thriceStringFirstPosChr(flags, *ctx.crt);
        if (pos == flags.end) {
            break;
        }
        ctx.flg.dat[pos - flags.beg] = true;
    }

    if (ctx.crt == ctx.fmt.end) {
        thriceError("No format conversion specifier!");
    }

    return ctx;
}

ThriceFormatContext thriceFormatNumber(ThriceFormatContext ctx, bool const wid)
{
    uint32_t num = 0;
    if (*ctx.crt == THRICE_FORMAT_WIDTH) {
        num = va_arg(ctx.arp, uint32_t);
        ctx.crt++;
    } else if (thriceDigit(*ctx.crt)) {
        do {
            ctx.crt++;
        } while (ctx.crt < ctx.fmt.end && thriceDigit(*ctx.crt));
        num = thriceStringParseU64(
            (ThriceString){.beg = ctx.fmt.beg, .end = ctx.crt});
    }
    if (ctx.crt == ctx.fmt.end) {
        thriceError("No format conversion specifier!");
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
    const ThriceString specifiers =
        thriceStringStatic(THRICE_FORMAT_SPECIFICATIONS);
    const uint8_t* pos = thriceStringFirstPosChr(specifiers, *ctx.crt);

    while (ctx.crt < ctx.fmt.end && pos < specifiers.end) {
        pos = thriceStringFirstPosChr(specifiers, *ctx.crt);
        ctx.crt++;
    }

    const ThriceString mod = {.beg = ctx.fmt.beg, .end = ctx.crt};

    if (ctx.crt == ctx.fmt.end) {
        thriceError("No format conversion specifier!");
    }

#define THRICE_FORMAT_MOD_COUNT 9
    const ThriceString mods[THRICE_FORMAT_MOD_COUNT] = {
        thriceStringStatic(""),
        thriceStringStatic("hh"),
        thriceStringStatic("h"),
        thriceStringStatic("l"),
        thriceStringStatic("ll"),
        thriceStringStatic("j"),
        thriceStringStatic("z"),
        thriceStringStatic("t"),
        thriceStringStatic("L")};
    ctx.mod = -1;
    for (size_t i = 0; i < THRICE_FORMAT_MOD_COUNT; i++) {
        if (thriceStringEquals(mods[i], mod)) {
            ctx.mod = i;
            break;
        }
    }
    if (ctx.mod == -1) {
        thriceError("Unkown length modifier!");
    }

    return ctx;
}

ThriceFormatContext thriceFormatChr(ThriceFormatContext ctx)
{
    if (ctx.mod != 0) {
        thriceError("Unsupported length modifier for formatting a char!");
    }
    const uint8_t chr = (uint8_t)va_arg(ctx.arp, int);
    if (ctx.flg.left) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, chr);
    }
    uint32_t wid = ctx.wid;
    while (--wid) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, ' ');
    }
    if (!ctx.flg.left) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, chr);
    }

    return ctx;
}

ThriceFormatContext thriceFormatString(ThriceFormatContext ctx)
{
    if (ctx.mod != 0) {
        thriceError("Unsupported length modifier for formatting a string!");
    }
    const ThriceString str = va_arg(ctx.arp, ThriceString);
    if (ctx.flg.left) {
        ctx.buf = thriceBufferAppendString(ctx.buf, str);
    }
    uint32_t wid = ctx.wid;
    while (--wid) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, ' ');
    }
    if (!ctx.flg.left) {
        ctx.buf = thriceBufferAppendString(ctx.buf, str);
    }

    return ctx;
}

ThriceBuffer thriceFormatAppend(ThriceBuffer buf000, ThriceString fmt000, ...)
{
    ThriceFormatContext ctx = {.buf = buf000, .fmt = fmt000};
    va_start(ctx.arp, fmt000);

    while (thriceStringLength(ctx.fmt)) {
        ctx = thriceFormatSkip(ctx);
        if (ctx.fmt.beg != ctx.crt) {
            ctx = thriceFormatConsume(ctx);
            continue;
        }

        ctx = thriceFormatEscape(ctx);
        if (ctx.fmt.beg != ctx.crt) {
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
                thriceError("Unsupported format conversion specifier!");
                break;
            default:
                thriceError("Unkown format conversion specifier!");
        }
    }

    va_end(ctx.arp);
    return ctx.buf;
}

#endif // THRICE_FORMAT
