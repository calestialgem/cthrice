// SPDX-FileCopyrightText: (C) 2022 Cem Geçgel <gecgelcem@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  unt8;
typedef uint16_t unt16;
typedef uint32_t unt32;
typedef uint64_t unt64;

typedef float  flt32;
typedef double flt64;

typedef const char ichr;
typedef const unt8 uchr;

typedef intptr_t  iptr;
typedef uintptr_t uptr;

#define CTHRICE_ERROR_LOGIC 1
#define CTHRICE_ERROR_ALLOC 2

void cthrice_error(ichr* msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(CTHRICE_ERROR_LOGIC);
}

void cthrice_error_alloc(void)
{
    fprintf(stderr, "ERROR: Could not allocate memory!\n");
    exit(CTHRICE_ERROR_ALLOC);
}

bool cthrice_whitespace(uchr chr)
{
    return chr == '\n' || chr == '\r' || chr == '\t' || chr == ' ';
}

bool cthrice_letter(uchr chr)
{
    return ('A' <= chr && 'Z' >= chr) || ('a' <= chr && 'z' >= chr);
}

bool cthrice_digit(uchr chr)
{
    return '0' <= chr && '9' >= chr;
}

bool cthrice_digit_bin(uchr chr)
{
    return '0' <= chr && '1' >= chr;
}

bool cthrice_digit_oct(uchr chr)
{
    return '0' <= chr && '7' >= chr;
}

bool cthrice_digit_hex(uchr chr)
{
    return ('0' <= chr && '9' >= chr) || ('A' <= chr && 'F' >= chr) ||
           ('a' <= chr && 'f' >= chr);
}

typedef struct {
    uchr* bgn;
    uchr* end;
} Cthrice_String;

Cthrice_String cthrice_string_static(ichr* cstr)
{
    Cthrice_String str = {.bgn = (uchr*)cstr, .end = (uchr*)cstr};
    while (*str.end) {
        str.end++;
    }
    return str;
}

uchr* cthrice_string_first_pos_chr(Cthrice_String str, uchr chr)
{
    uchr* pos = str.bgn;
    while (pos < str.end && *pos != chr) {
        pos++;
    }
    return pos;
}

uptr cthrice_string_length(Cthrice_String str)
{
    return str.end - str.bgn;
}

bool cthrice_string_equals(Cthrice_String lhs, Cthrice_String rhs)
{
    uptr len = cthrice_string_length(lhs);
    if (len != cthrice_string_length(rhs)) {
        return false;
    }
    for (uptr i = 0; i < len; i++) {
        if (lhs.bgn[i] != rhs.bgn[i]) {
            return false;
        }
    }
    return true;
}

void cthrice_string_println(Cthrice_String str)
{
    printf("%.*s\n", (int)cthrice_string_length(str), str.bgn);
}

Cthrice_String cthrice_string_trim(Cthrice_String str)
{
    while (str.bgn < str.end && cthrice_whitespace(*str.bgn)) {
        str.bgn++;
    }
    return str;
}

Cthrice_String cthrice_string_first_word(Cthrice_String str)
{
    uchr* end = str.bgn;

    while (end < str.end && !cthrice_whitespace(*end)) {
        end++;
    }

    str.end = end;
    return str;
}

Cthrice_String cthrice_string_part(Cthrice_String str, uptr boff, uptr eoff)
{
    str.end = str.bgn + eoff;
    str.bgn += boff;
    return str;
}

Cthrice_String cthrice_string_skip(Cthrice_String str, uptr amt)
{
    str.bgn += amt;
    return str;
}

unt64 cthrice_string_parse_u64(Cthrice_String str)
{
    unt64 res = 0;

    for (uchr* i = str.bgn; i < str.end; i++) {
        const unt64 BASE = 10;
        res *= BASE;
        res += *i - '0';
    }

    return res;
}

typedef struct {
    unt8* restrict bgn;
    unt8* restrict end;
    unt8* restrict lst;
} Cthrice_Buffer;

Cthrice_Buffer cthrice_buffer_create(uptr cap)
{
    unt8* restrict mem = malloc(cap);
    if (!mem) {
        cthrice_error_alloc();
    }
    return (Cthrice_Buffer){.bgn = mem, .end = mem, .lst = mem + cap};
}

Cthrice_Buffer cthrice_buffer_destroy(Cthrice_Buffer bfr)
{
    free(bfr.bgn);
    return (Cthrice_Buffer){.bgn = 0, .end = 0, .lst = 0};
}

uptr cthrice_buffer_size(Cthrice_Buffer bfr)
{
    return bfr.end - bfr.bgn;
}

uptr cthrice_buffer_capacity(Cthrice_Buffer bfr)
{
    return bfr.lst - bfr.bgn;
}

uptr cthrice_buffer_space(Cthrice_Buffer bfr)
{
    return bfr.lst - bfr.end;
}

Cthrice_String cthrice_buffer_view(Cthrice_Buffer bfr)
{
    return (Cthrice_String){.bgn = bfr.bgn, .end = bfr.end};
}

Cthrice_Buffer cthrice_buffer_clear(Cthrice_Buffer bfr)
{
    bfr.end = bfr.bgn;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_recap(Cthrice_Buffer bfr, uptr cap)
{
    if (!cap) {
        return cthrice_buffer_destroy(bfr);
    }
    if (!bfr.bgn) {
        return cthrice_buffer_create(cap);
    }
    unt8* restrict mem = realloc(bfr.bgn, cap);
    if (!mem) {
        cthrice_error_alloc();
    }
    bfr.lst = mem + cap;
    bfr.end = mem + min(cap, cthrice_buffer_size(bfr));
    bfr.bgn = mem;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_unt8(Cthrice_Buffer bfr, unt8 chr)
{
    if (cthrice_buffer_space(bfr) < 1) {
        uptr cap = cthrice_buffer_capacity(bfr);
        bfr      = cthrice_buffer_recap(bfr, max(1, 2 * cap));
    }

    *(bfr.end) = chr;
    bfr.end++;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_unt64(Cthrice_Buffer bfr, unt64 unt)
{
    const unt64 BASE  = 10;
    unt64       place = BASE;

    while (1 != place) {
        if (unt > place) {
            place *= BASE;
            break;
        }
        place /= BASE;
        bfr = cthrice_buffer_append_unt8(bfr, '0' + (unt8)(unt / place));
        unt %= place;
    }

    return bfr;
}

Cthrice_Buffer
cthrice_buffer_append_string(Cthrice_Buffer bfr, Cthrice_String str)
{
    uptr spc = cthrice_buffer_space(bfr);
    uptr len = cthrice_string_length(str);
    if (spc < len) {
        uptr cap = cthrice_buffer_capacity(bfr);
        bfr      = cthrice_buffer_recap(bfr, cap + len - spc);
    }

    memcpy(bfr.end, str.bgn, len);
    bfr.end += len;
    return bfr;
}

Cthrice_Buffer cthrice_buffer_append_file(Cthrice_Buffer bfr, ichr* name)
{
    FILE* file = fopen(name, "r");
    if (!file) {
        cthrice_error("Could not open file!");
    }

    const uptr CHUNK = 1024;
    uptr       wrt   = 0;

    do {
        uptr spc = cthrice_buffer_space(bfr);
        if (spc < CHUNK) {
            bfr = cthrice_buffer_recap(bfr, CHUNK - spc);
        }
        wrt = fread(bfr.end, 1, CHUNK, file);
        bfr.end += wrt;
    } while (CHUNK == wrt);

    if (!feof(file)) {
        cthrice_error("Problem while reading!");
    }

    fclose(file);
    return bfr;
}

uchr CTHRICE_FORMAT_INTRODUCTORY     = '%';
uchr CTHRICE_FORMAT_WIDTH            = '*';
uchr CTHRICE_FORMAT_PRECISION        = '.';
ichr CTHRICE_FORMAT_FLAGS[]          = "-+ #0";
ichr CTHRICE_FORMAT_SPECIFICATIONS[] = "cs";

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

Cthrice_Format_Context (*const CTHRICE_FORMAT_FUNCTIONS[])(
    Cthrice_Format_Context) = {&cthrice_format_chr, &cthrice_format_string};

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

typedef enum {
    CTHRICE_TOKEN_SEMICOLON,
    CTHRICE_TOKEN_OPENING_BRACKET,
    CTHRICE_TOKEN_CLOSING_BRACKET,
    CTHRICE_TOKEN_OPENING_SQUARE_BRACKET,
    CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET,
    CTHRICE_TOKEN_OPENING_CURLY_BRACKET,
    CTHRICE_TOKEN_CLOSING_CURLY_BRACKET,
    CTHRICE_TOKEN_INTEGER,
    CTHRICE_TOKEN_KEYWORD_INT8,
    CTHRICE_TOKEN_KEYWORD_INT16,
    CTHRICE_TOKEN_KEYWORD_INT32,
    CTHRICE_TOKEN_KEYWORD_INT64,
    CTHRICE_TOKEN_KEYWORD_UNT8,
    CTHRICE_TOKEN_KEYWORD_UNT16,
    CTHRICE_TOKEN_KEYWORD_UNT32,
    CTHRICE_TOKEN_KEYWORD_UNT64,
    CTHRICE_TOKEN_KEYWORD_STR,
    CTHRICE_TOKEN_KEYWORD_RETURN,
    CTHRICE_TOKEN_IDENTIFIER,
    CTHRICE_TOKEN_END_OF_FILE
} Cthrice_Token_Type;

#define CTHRICE_KEYWORD_COUNT 10
ichr* const CTHRICE_KEYWORDS[CTHRICE_KEYWORD_COUNT] = {
    "int8",
    "int16",
    "int32",
    "int64",
    "unt8",
    "unt16",
    "unt32",
    "unt64",
    "str",
    "return"};

ichr* cthrice_token_static_name(Cthrice_Token_Type typ)
{
    switch (typ) {
        case CTHRICE_TOKEN_SEMICOLON:
            return "semicolon";
        case CTHRICE_TOKEN_OPENING_BRACKET:
            return "opening-bracket";
        case CTHRICE_TOKEN_CLOSING_BRACKET:
            return "closing-bracket";
        case CTHRICE_TOKEN_OPENING_SQUARE_BRACKET:
            return "opening-square-bracket";
        case CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET:
            return "closing-square-bracket";
        case CTHRICE_TOKEN_OPENING_CURLY_BRACKET:
            return "opening-curly-bracket";
        case CTHRICE_TOKEN_CLOSING_CURLY_BRACKET:
            return "closing-curly-bracket";
        case CTHRICE_TOKEN_INTEGER:
            return "integer";
        case CTHRICE_TOKEN_KEYWORD_INT8:
        case CTHRICE_TOKEN_KEYWORD_INT16:
        case CTHRICE_TOKEN_KEYWORD_INT32:
        case CTHRICE_TOKEN_KEYWORD_INT64:
        case CTHRICE_TOKEN_KEYWORD_UNT8:
        case CTHRICE_TOKEN_KEYWORD_UNT16:
        case CTHRICE_TOKEN_KEYWORD_UNT32:
        case CTHRICE_TOKEN_KEYWORD_UNT64:
        case CTHRICE_TOKEN_KEYWORD_STR:
        case CTHRICE_TOKEN_KEYWORD_RETURN:
            return CTHRICE_KEYWORDS[typ - CTHRICE_TOKEN_KEYWORD_INT8];
        case CTHRICE_TOKEN_IDENTIFIER:
            return "identifier";
        case CTHRICE_TOKEN_END_OF_FILE:
            return "end-of-file";
        default:
            return "unkown-token";
    }
}

Cthrice_String cthrice_token_name(Cthrice_Token_Type typ)
{
    return cthrice_string_static(cthrice_token_static_name(typ));
}

typedef struct {
    Cthrice_Token_Type typ;
    Cthrice_String     val;
} Cthrice_Token;

void cthrice_token_print(Cthrice_Token tkn)
{
    Cthrice_Buffer bfr = {};
    bfr = cthrice_format_append(bfr, "%s", cthrice_token_name(tkn.typ));
    switch (tkn.typ) {
        case CTHRICE_TOKEN_SEMICOLON:
        case CTHRICE_TOKEN_OPENING_BRACKET:
        case CTHRICE_TOKEN_CLOSING_BRACKET:
        case CTHRICE_TOKEN_OPENING_SQUARE_BRACKET:
        case CTHRICE_TOKEN_CLOSING_SQUARE_BRACKET:
        case CTHRICE_TOKEN_OPENING_CURLY_BRACKET:
        case CTHRICE_TOKEN_CLOSING_CURLY_BRACKET:
        case CTHRICE_TOKEN_KEYWORD_INT8:
        case CTHRICE_TOKEN_KEYWORD_INT16:
        case CTHRICE_TOKEN_KEYWORD_INT32:
        case CTHRICE_TOKEN_KEYWORD_INT64:
        case CTHRICE_TOKEN_KEYWORD_UNT8:
        case CTHRICE_TOKEN_KEYWORD_UNT16:
        case CTHRICE_TOKEN_KEYWORD_UNT32:
        case CTHRICE_TOKEN_KEYWORD_UNT64:
        case CTHRICE_TOKEN_KEYWORD_STR:
        case CTHRICE_TOKEN_KEYWORD_RETURN:
        case CTHRICE_TOKEN_END_OF_FILE:
            break;
        case CTHRICE_TOKEN_INTEGER:
        case CTHRICE_TOKEN_IDENTIFIER:
        default:
            bfr = cthrice_format_append(bfr, " {%s}", tkn.val);
    }
    cthrice_string_println(cthrice_buffer_view(bfr));
    bfr = cthrice_buffer_destroy(bfr);
}

typedef struct {
    Cthrice_Token  lst;
    Cthrice_String src;
} Cthrice_Lexer;

Cthrice_String cthrice_lexer_word(Cthrice_String src)
{
    Cthrice_String trm = cthrice_string_trim(src);
    return cthrice_string_first_word(trm);
}

Cthrice_Lexer cthrice_lexer_create(
    Cthrice_Token_Type typ,
    Cthrice_String     val,
    Cthrice_String     src)
{
    return (Cthrice_Lexer){
        .lst = {    .typ = typ,     .val = val},
        .src = {.bgn = val.end, .end = src.end}
    };
}

Cthrice_Lexer cthrice_lexer_number(Cthrice_String word, Cthrice_String src)
{
    uptr len = cthrice_string_length(word);

    if (len == 1) {
        return cthrice_lexer_create(CTHRICE_TOKEN_INTEGER, word, src);
    }

    bool (*digit)(uchr) = &cthrice_digit;
    uchr* end           = word.bgn + 1;

    if ('0' == *word.bgn) {
        switch (*end) {
            case 'b':
                digit = &cthrice_digit_bin;
                break;
            case 'o':
                digit = &cthrice_digit_oct;
                break;
            case 'x':
                digit = &cthrice_digit_hex;
                break;
            case '_':
                break;
            default:
                return cthrice_lexer_create(
                    CTHRICE_TOKEN_INTEGER,
                    cthrice_string_part(word, 0, 1),
                    src);
        }

        if (++end >= word.end) {
            cthrice_error("Number ended after the base indicator!");
        }
    }

    bool sep = false;
    while (end < word.end && ((*end == '_' && sep) || digit(*end))) {
        sep = *end != '_';
        end++;
    }
    if (*end == '_') {
        end--;
    }

    return cthrice_lexer_create(
        CTHRICE_TOKEN_INTEGER,
        (Cthrice_String){.bgn = word.bgn, .end = end},
        src);
}

bool cthrice_lexer_id_char(uchr chr)
{
    return chr == '_' || cthrice_letter(chr);
}

Cthrice_Lexer cthrice_lexer_next(Cthrice_String src)
{
    Cthrice_String word = cthrice_lexer_word(src);

    if (!cthrice_string_length(word)) {
        return cthrice_lexer_create(CTHRICE_TOKEN_END_OF_FILE, word, src);
    }

    { // Check for punctuation.
        Cthrice_String puncs = cthrice_string_static(";()[]{}");
        uchr*          pos   = cthrice_string_first_pos_chr(puncs, *word.bgn);

        if (pos < puncs.end) {
            return cthrice_lexer_create(
                CTHRICE_TOKEN_SEMICOLON + (pos - puncs.bgn),
                cthrice_string_part(word, 0, 1),
                src);
        }
    }

    // Check for number.
    if (cthrice_digit(*word.bgn)) {
        return cthrice_lexer_number(word, src);
    }

    // Check for identifier or keyword.
    if (cthrice_lexer_id_char(*word.bgn)) {
        uchr* end = word.bgn + 1;
        while (end < word.end &&
               (cthrice_lexer_id_char(*end) || cthrice_digit(*end))) {
            end++;
        }
        Cthrice_String val = {.bgn = word.bgn, .end = end};

        for (uptr i = 0; i < CTHRICE_KEYWORD_COUNT; i++) {
            if (cthrice_string_equals(
                    cthrice_string_static(CTHRICE_KEYWORDS[i]),
                    val)) {
                return cthrice_lexer_create(
                    CTHRICE_TOKEN_KEYWORD_INT8 + i,
                    val,
                    src);
            }
        }
        return cthrice_lexer_create(CTHRICE_TOKEN_IDENTIFIER, val, src);
    }

    Cthrice_String val = cthrice_string_part(word, 0, 1);
    return cthrice_lexer_create(-1, val, src);
}

typedef struct {
    Cthrice_Token* restrict bgn;
    Cthrice_Token* restrict end;
} Cthrice_Lex;

Cthrice_Lex cthrice_lex(Cthrice_String src)
{
    // Create a buffer.
    Cthrice_Buffer bfr   = {};
    Cthrice_Lexer  lexer = {};

    // Append all the tokens to the buffer.
    do {
        // Lex the next token.
        lexer = cthrice_lexer_next(src);
        src   = lexer.src;

        // Append the token.
        Cthrice_String str = {
            .bgn = (uchr*)&lexer.lst,
            .end = (uchr*)&lexer.lst + sizeof(lexer.lst)};
        bfr = cthrice_buffer_append_string(bfr, str);

        // Continue until the end of the file is reached.
    } while (lexer.lst.typ != CTHRICE_TOKEN_END_OF_FILE);

    // Create an array for all the tokens.
    uptr size                   = cthrice_buffer_size(bfr);
    Cthrice_Token* restrict mem = malloc(size);

    // Copy the tokens from the buffer to the new memory.
    memcpy(mem, bfr.bgn, size);
    cthrice_buffer_destroy(bfr);

    uptr len = size / sizeof(Cthrice_Token);
    return (Cthrice_Lex){.bgn = mem, .end = mem + len};
}

Cthrice_Lex cthrice_lex_destory(Cthrice_Lex lex)
{
    free(lex.bgn);
    return (Cthrice_Lex){.bgn = 0, .end = 0};
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        cthrice_error("Provide a Thrice file!");
    }
    const uptr     CAPPACITY = 1024;
    Cthrice_Buffer buf       = cthrice_buffer_create(CAPPACITY);

    for (int i = 1; i < argc; i++) {
        Cthrice_String name = cthrice_string_static(argv[i]);
        cthrice_format_println("Name of the file: %s", name);

        buf = cthrice_buffer_clear(buf);
        buf = cthrice_buffer_append_file(buf, argv[i]);

        Cthrice_String src = cthrice_buffer_view(buf);
        Cthrice_Lex    lex = cthrice_lex(src);

        for (const Cthrice_Token* i = lex.bgn; i < lex.end; i++) {
            cthrice_token_print(*i);
        }

        cthrice_lex_destory(lex);
    }

    cthrice_buffer_destroy(buf);
    return EXIT_SUCCESS;
}
