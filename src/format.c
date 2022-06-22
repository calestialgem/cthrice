// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef THRICE_FORMAT
#define THRICE_FORMAT

#include "buffer.c"
#include "error.c"
#include "string.c"

#include <stdarg.h>
#include <stdint.h>

const uint8_t     fmtmeta  = '%';
const uint8_t     fmtdyna  = '*';
const uint8_t     fmtprec  = '.';
const char* const fmtflags = "-+ #0";
const char* const fmtspec  = "csdioxXufFeEaAgGnP";

union fmtf {
    bool dat[5];
    struct {
        bool left;
        bool plus;
        bool spce;
        bool altn;
        bool zero;
    };
};

struct fmtc {
    va_list        arp;
    Buffer     buf;
    struct str     fmt;
    const uint8_t* crt;
    union fmtf     flg;
    uint32_t       wid;
    uint32_t       pre;
    size_t         mod;
};

struct fmtc thriceFormatConsume(struct fmtc ctx)
{
    ctx.fmt.beg = ctx.crt;
    return ctx;
}

struct fmtc thriceFormatSkip(struct fmtc ctx)
{
    ctx.crt = thriceStringFirstPosChr(ctx.fmt, fmtmeta);
    ctx.buf = thriceBufferAppendString(
        ctx.buf,
        (struct str){.beg = ctx.fmt.beg, .end = ctx.crt});
    return ctx;
}

struct fmtc thriceFormatEscape(struct fmtc ctx)
{
    if (thriceStringLength(ctx.fmt) < 2) {
        thriceError("No format conversion specifier!");
    }
    ctx.crt++;
    ctx = thriceFormatConsume(ctx);
    if (*ctx.crt == fmtmeta) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, fmtmeta);
        ctx.crt++;
    }
    return ctx;
}

struct fmtc thriceFormatFlags(struct fmtc ctx)
{
    const struct str flags = thriceStringStatic(fmtflags);
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

struct fmtc thriceFormatNumber(struct fmtc ctx, bool const wid)
{
    uint32_t num = 0;
    if (*ctx.crt == fmtdyna) {
        num = va_arg(ctx.arp, uint32_t);
        ctx.crt++;
    } else if (thriceDigit(*ctx.crt)) {
        do {
            ctx.crt++;
        } while (ctx.crt < ctx.fmt.end && thriceDigit(*ctx.crt));
        num = thriceStringParseU64(
            (struct str){.beg = ctx.fmt.beg, .end = ctx.crt});
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

struct fmtc thriceFormatModifier(struct fmtc ctx)
{
    const struct str specifiers = thriceStringStatic(fmtspec);
    const uint8_t*   pos        = thriceStringFirstPosChr(specifiers, *ctx.crt);

    while (ctx.crt < ctx.fmt.end && pos < specifiers.end) {
        pos = thriceStringFirstPosChr(specifiers, *ctx.crt);
        ctx.crt++;
    }

    const struct str mod = {.beg = ctx.fmt.beg, .end = ctx.crt};

    if (ctx.crt == ctx.fmt.end) {
        thriceError("No format conversion specifier!");
    }

#define THRICE_FORMAT_MOD_COUNT 9
    const struct str mods[THRICE_FORMAT_MOD_COUNT] = {
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

struct fmtc thriceFormatChr(struct fmtc ctx)
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

struct fmtc thriceFormatString(struct fmtc ctx)
{
    if (ctx.mod != 0) {
        thriceError("Unsupported length modifier for formatting a string!");
    }
    const struct str str = va_arg(ctx.arp, struct str);
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

Buffer thriceFormatAppend(Buffer buf000, struct str fmt000, ...)
{
    struct fmtc ctx = {.buf = buf000, .fmt = fmt000};
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
