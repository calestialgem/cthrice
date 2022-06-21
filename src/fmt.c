// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CTHR_FMT
#define CTHR_FMT

#include "buf.c"
#include "err.c"
#include "str.c"

#include <stdarg.h>
#include <stdint.h>

const uint8_t     fmtmeta  = '%';
const uint8_t     fmtdyna  = '*';
const uint8_t     fmtprec  = '.';
const char* const fmtflags = "-+ #0";

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
    struct buf     buf;
    struct str     fmt;
    const uint8_t* crt;
    union fmtf     flg;
    uint32_t       wid;
    uint32_t       pre;
};

struct fmtc cthr_fmt_consume(struct fmtc ctx)
{
    ctx.fmt.beg = ctx.crt;
    return ctx;
}

struct fmtc cthr_fmt_skip(struct fmtc ctx)
{
    ctx.crt = thriceStringFirstPosChar(ctx.fmt, fmtmeta);
    ctx.buf = thriceBufferAppendString(
        ctx.buf,
        (struct str){.beg = ctx.fmt.beg, .end = ctx.crt});
    return ctx;
}

struct fmtc cthr_fmt_escape(struct fmtc ctx)
{
    if (thriceStringLength(ctx.fmt) < 2) {
        thriceError("No format conversion specifier!");
    }
    ctx.crt++;
    ctx = cthr_fmt_consume(ctx);
    if (*ctx.crt == fmtmeta) {
        ctx.buf = thriceBufferAppendU8(ctx.buf, fmtmeta);
        ctx.crt++;
    }
    return ctx;
}

struct fmtc cthr_fmt_flags(struct fmtc ctx)
{
    const struct str flags = thriceStringStatic(fmtflags);
    for (; ctx.crt < ctx.fmt.end; ctx.crt++) {
        const uint8_t* pos = thriceStringFirstPosChar(flags, *ctx.crt);
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

struct fmtc cthr_fmt_nums(struct fmtc ctx, bool const wid)
{
    uint32_t num = 0;
    if (*ctx.crt == fmtdyna) {
        num = va_arg(ctx.arp, uint32_t);
        ctx.crt++;
    } else if (cthr_str_digit(*ctx.crt)) {
        do {
            ctx.crt++;
        } while (ctx.crt < ctx.fmt.end && cthr_str_digit(*ctx.crt));
        num = cthr_str_u32((struct str){.beg = ctx.fmt.beg, .end = ctx.crt});
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

struct buf cthr_fmt_append(struct buf buf000, struct str fmt000, ...)
{
    struct fmtc ctx = {.buf = buf000, .fmt = fmt000};
    va_start(ctx.arp, fmt000);

    while (thriceStringLength(ctx.fmt)) {
        ctx = cthr_fmt_skip(ctx);
        if (ctx.fmt.beg != ctx.crt) {
            ctx = cthr_fmt_consume(ctx);
            continue;
        }

        ctx = cthr_fmt_escape(ctx);
        if (ctx.fmt.beg != ctx.crt) {
            ctx = cthr_fmt_consume(ctx);
            continue;
        }

        ctx = cthr_fmt_flags(ctx);

        uint32_t wid       = cthr_fmt_u32(argp, &beg, fmt);
        uint32_t precision = 0;

        if (*beg == '.') {
            precision = cthr_fmt_u32(argp, &beg, fmt);
        }

        struct str       lmod       = {.beg = beg, .end = beg};
        const struct str specifiers = thriceStringStatic("csdioxXufFeEaAgGnP");
        const uint8_t*   pos        = thriceStringFirstPosChar(specifiers, *lmod.end);

        while (lmod.end < fmt.end && pos < specifiers.end) {
            pos = thriceStringFirstPosChar(specifiers, *lmod.end);
            lmod.end++;
        }

        beg = lmod.end;
        if (beg == fmt.end) {
            thriceError("No format conversion specifier!");
        }

#define CTHR_FMT_SPECIFIER_COUNT 9
        const struct str lmods[CTHR_FMT_SPECIFIER_COUNT] = {
            thriceStringStatic(""),
            thriceStringStatic("hh"),
            thriceStringStatic("h"),
            thriceStringStatic("l"),
            thriceStringStatic("ll"),
            thriceStringStatic("j"),
            thriceStringStatic("z"),
            thriceStringStatic("t"),
            thriceStringStatic("L")};
        size_t lmodi = -1;
        for (size_t i = 0; i < CTHR_FMT_SPECIFIER_COUNT; i++) {
            if (thriceStringEquals(lmods[i], lmod)) {
                lmodi = i;
                break;
            }
        }
        if (lmodi == -1) {
            thriceError("Unkown length modifier!");
        }

        switch (*beg) {
            case 'c':
                if (lmodi != 0) {
                    thriceError(
                        "Unsupported length modifier for formatting a char!");
                }
                const uint8_t chr = (uint8_t)va_arg(argp, int);
                if (enabled & left) {
                    buf = cthr_buf_append_char(buf, chr);
                }
                while (--wid) {
                    buf = cthr_buf_append_char(buf, ' ');
                }
                if (!(enabled & left)) {
                    buf = cthr_buf_append_char(buf, chr);
                }
                break;
            case 's':
                if (lmodi != 0) {
                    thriceError(
                        "Unsupported length modifier for formatting a string!");
                }
                const struct str str = va_arg(argp, struct str);
                if (enabled & left) {
                    buf = cthr_buf_append(buf, str);
                }
                wid -= thriceStringLength(str);
                while (wid--) {
                    buf = cthr_buf_append_char(buf, ' ');
                }
                if (!(enabled & left)) {
                    buf = cthr_buf_append(buf, str);
                }
                break;
            case 'd':
            case 'i':
            case 'o':
            case 'x':
            case 'X':
            case 'u':
            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'a':
            case 'A':
            case 'g':
            case 'G':
            case 'n':
            case 'p':
            default:
                thriceError("Unkown format conversion specifier!");
        }

        fmt.beg = beg++;
    }

    va_end(argp);
    return buf;
}

#endif // CTHR_FMT
