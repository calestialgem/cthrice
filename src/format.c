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

ThriceFormatContext ThriceFormatConsume(ThriceFormatContext ctx)
{
    ctx.fmt.beg = ctx.crt;
    return ctx;
}

ThriceFormatContext ThriceFormatSkip(ThriceFormatContext ctx)
{
    ctx.crt = ThriceStringFirstPosChr(ctx.fmt, THRICE_FORMAT_INTRODUCTORY);
    ctx.buf = ThriceBufferAppendString(
        ctx.buf,
        (ThriceString){.beg = ctx.fmt.beg, .end = ctx.crt});
    return ctx;
}

ThriceFormatContext ThriceFormatEscape(ThriceFormatContext ctx)
{
    if (ThriceStringLength(ctx.fmt) < 2) {
        ThriceError("No format conversion specifier!");
    }
    ctx.crt++;
    ctx = ThriceFormatConsume(ctx);
    if (*ctx.crt == THRICE_FORMAT_INTRODUCTORY) {
        ctx.buf = ThriceBufferAppendU8(ctx.buf, THRICE_FORMAT_INTRODUCTORY);
        ctx.crt++;
    }
    return ctx;
}

ThriceFormatContext ThriceFormatFlags(ThriceFormatContext ctx)
{
    const ThriceString flags = ThriceStringStatic(THRICE_FORMAT_FLAGS);
    for (; ctx.crt < ctx.fmt.end; ctx.crt++) {
        const uint8_t* pos = ThriceStringFirstPosChr(flags, *ctx.crt);
        if (pos == flags.end) {
            break;
        }
        ctx.flg.dat[pos - flags.beg] = true;
    }

    if (ctx.crt == ctx.fmt.end) {
        ThriceError("No format conversion specifier!");
    }

    return ctx;
}

ThriceFormatContext ThriceFormatNumber(ThriceFormatContext ctx, bool const wid)
{
    uint32_t num = 0;
    if (*ctx.crt == THRICE_FORMAT_WIDTH) {
        num = va_arg(ctx.arp, uint32_t);
        ctx.crt++;
    } else if (ThriceDigit(*ctx.crt)) {
        do {
            ctx.crt++;
        } while (ctx.crt < ctx.fmt.end && ThriceDigit(*ctx.crt));
        num = ThriceStringParseU64(
            (ThriceString){.beg = ctx.fmt.beg, .end = ctx.crt});
    }
    if (ctx.crt == ctx.fmt.end) {
        ThriceError("No format conversion specifier!");
    }

    if (wid) {
        ctx.wid = num;
    } else {
        ctx.pre = num;
    }
    return ctx;
}

ThriceFormatContext ThriceFormatModifier(ThriceFormatContext ctx)
{
    const ThriceString specifiers =
        ThriceStringStatic(THRICE_FORMAT_SPECIFICATIONS);
    const uint8_t* pos = ThriceStringFirstPosChr(specifiers, *ctx.crt);

    while (ctx.crt < ctx.fmt.end && pos < specifiers.end) {
        pos = ThriceStringFirstPosChr(specifiers, *ctx.crt);
        ctx.crt++;
    }

    const ThriceString mod = {.beg = ctx.fmt.beg, .end = ctx.crt};

    if (ctx.crt == ctx.fmt.end) {
        ThriceError("No format conversion specifier!");
    }

#define THRICE_FORMAT_MOD_COUNT 9
    const ThriceString mods[THRICE_FORMAT_MOD_COUNT] = {
        ThriceStringStatic(""),
        ThriceStringStatic("hh"),
        ThriceStringStatic("h"),
        ThriceStringStatic("l"),
        ThriceStringStatic("ll"),
        ThriceStringStatic("j"),
        ThriceStringStatic("z"),
        ThriceStringStatic("t"),
        ThriceStringStatic("L")};
    ctx.mod = -1;
    for (size_t i = 0; i < THRICE_FORMAT_MOD_COUNT; i++) {
        if (ThriceStringEquals(mods[i], mod)) {
            ctx.mod = i;
            break;
        }
    }
    if (ctx.mod == -1) {
        ThriceError("Unkown length modifier!");
    }

    return ctx;
}

ThriceFormatContext ThriceFormatChr(ThriceFormatContext ctx)
{
    if (ctx.mod != 0) {
        ThriceError("Unsupported length modifier for formatting a char!");
    }
    const uint8_t chr = (uint8_t)va_arg(ctx.arp, int);
    if (ctx.flg.left) {
        ctx.buf = ThriceBufferAppendU8(ctx.buf, chr);
    }
    uint32_t wid = ctx.wid;
    while (--wid) {
        ctx.buf = ThriceBufferAppendU8(ctx.buf, ' ');
    }
    if (!ctx.flg.left) {
        ctx.buf = ThriceBufferAppendU8(ctx.buf, chr);
    }

    return ctx;
}

ThriceFormatContext ThriceFormatString(ThriceFormatContext ctx)
{
    if (ctx.mod != 0) {
        ThriceError("Unsupported length modifier for formatting a string!");
    }
    const ThriceString str = va_arg(ctx.arp, ThriceString);
    if (ctx.flg.left) {
        ctx.buf = ThriceBufferAppendString(ctx.buf, str);
    }
    uint32_t wid = ctx.wid;
    while (--wid) {
        ctx.buf = ThriceBufferAppendU8(ctx.buf, ' ');
    }
    if (!ctx.flg.left) {
        ctx.buf = ThriceBufferAppendString(ctx.buf, str);
    }

    return ctx;
}

ThriceBuffer ThriceFormatAppend(ThriceBuffer buf000, ThriceString fmt000, ...)
{
    ThriceFormatContext ctx = {.buf = buf000, .fmt = fmt000};
    va_start(ctx.arp, fmt000);

    while (ThriceStringLength(ctx.fmt)) {
        ctx = ThriceFormatSkip(ctx);
        if (ctx.fmt.beg != ctx.crt) {
            ctx = ThriceFormatConsume(ctx);
            continue;
        }

        ctx = ThriceFormatEscape(ctx);
        if (ctx.fmt.beg != ctx.crt) {
            ctx = ThriceFormatConsume(ctx);
            continue;
        }

        ctx = ThriceFormatFlags(ctx);
        ctx = ThriceFormatConsume(ctx);

        ctx = ThriceFormatNumber(ctx, true);
        ctx = ThriceFormatConsume(ctx);

        ctx = ThriceFormatNumber(ctx, false);
        ctx = ThriceFormatConsume(ctx);

        ctx = ThriceFormatModifier(ctx);
        ctx = ThriceFormatConsume(ctx);

        switch (*ctx.crt) {
            case 'c':
                ctx = ThriceFormatChr(ctx);
                break;
            case 's':
                ctx = ThriceFormatString(ctx);
                break;
            case 'd':
            case 'i':
                // ctx = ThriceFormatSigned(ctx);
                // break;
            case 'o':
                // ctx = ThriceFormatOct(ctx);
                // break;
            case 'x':
            case 'X':
                // ctx = ThriceFormatHex(ctx);
                // break;
            case 'u':
                // ctx = ThriceFormatUnsigned(ctx);
                // break;
            case 'f':
            case 'F':
                // ctx = ThriceFormatFloat(ctx);
                // break;
            case 'e':
            case 'E':
                // ctx = ThriceFormatFloatE(ctx);
                // break;
            case 'a':
            case 'A':
                // ctx = ThriceFormatFloatA(ctx);
                // break;
            case 'g':
            case 'G':
                // ctx = ThriceFormatFloatG(ctx);
                // break;
            case 'n':
                // ctx = ThriceFormatCount(ctx);
                // break;
            case 'p':
                // ctx = ThriceFormatPointer(ctx);
                // break;
                ThriceError("Unsupported format conversion specifier!");
                break;
            default:
                ThriceError("Unkown format conversion specifier!");
        }
    }

    va_end(ctx.arp);
    return ctx.buf;
}

#endif // THRICE_FORMAT
