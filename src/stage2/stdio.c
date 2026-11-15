/*
 * this file is almost entirely vibe coded
 */
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <console.h>

/*
 * Formatted output.
 *
 *   %[flags][width][.precision][length]conversion
 *
 *   flags:      - + space 0 #
 *   width:      digits or *
 *   precision:  .digits or .*
 *   length:     hh h l ll j z t
 *   conversion: d i u o x X b c s p %
 *
 * %b is the C23 binary conversion. Floating point (%f, %e, %g, %a), %n and
 * wide-character conversions (%lc, %ls) are deliberately unsupported: they
 * would pull in soft float or locale/wide-char machinery, or are simply a
 * bad idea in a bootloader.
 *
 * Output goes through a Sink so the same formatter can drive the VGA
 * console (printf) or a caller supplied buffer (snprintf).
 */

typedef struct
{
    void (*write)(void *ctx, const char *s, size_t len);
    void *ctx;
    size_t written;	//bytes emitted so far
} Sink;

typedef enum
{
    LEN_NONE,
    LEN_HH,
    LEN_H,
    LEN_L,
    LEN_LL,
    LEN_J,
    LEN_Z,
    LEN_T
} Length;

typedef struct
{
    bool minus;		// '-' left justify
    bool plus;		// '+' always print a sign
    bool space;		// ' ' print a space for positive values
    bool hash;		// '#' alternative form (0x, 0, 0b)
    bool zero;		// '0' zero pad
    int width;		// 0 when not specified
    int prec;		// -1 when not specified
    Length len;
} Spec;

static void SinkWrite(Sink *out, const char *s, size_t len)
{
    out->write(out->ctx, s, len);
    out->written += len;
}

static void SinkChar(Sink *out, char c)
{
    SinkWrite(out, &c, 1);
}

static void SinkRepeat(Sink *out, char c, int count)
{
    while (count-- > 0)
	SinkChar(out, c);
}

static void EmitText(Sink *out, const char *s, size_t len, const Spec *spec)
{
    int pad = spec->width > (int)len ? spec->width - (int)len : 0;

    if (!spec->minus)
	SinkRepeat(out, ' ', pad);

    SinkWrite(out, s, len);

    if (spec->minus)
	SinkRepeat(out, ' ', pad);
}

//64 bit value / small base without pulling in libgcc's __udivdi3: restoring
//division one bit at a time. The remainder always stays below base, so the
//32 bit r can never overflow.
static uint64_t DivMod64(uint64_t value, uint32_t base, uint32_t *rem)
{
    uint64_t quotient = 0;
    uint32_t r = 0;

    for (int i = 63; i >= 0; i--)
    {
	r = (r << 1) | (uint32_t)((value >> i) & 1u);

	if (r >= base)
	{
	    r -= base;
	    quotient |= (uint64_t)1 << i;
	}
    }

    *rem = r;
    return quotient;
}

static void EmitNumber(Sink *out, uint64_t magnitude, int base, bool upper,
		       bool negative, const Spec *spec)
{
    static const char lower[] = "0123456789abcdef";
    static const char upperDigits[] = "0123456789ABCDEF";
    const char *digits = upper ? upperDigits : lower;
    char reversed[72];
    int digitCount = 0;
    bool isZero = magnitude == 0;

    if (isZero && spec->prec != 0)
	reversed[digitCount++] = '0';

    while (magnitude != 0 && digitCount < (int)sizeof(reversed))
    {
	uint32_t rem;
	magnitude = DivMod64(magnitude, (uint32_t)base, &rem);
	reversed[digitCount++] = digits[rem];
    }

    //"%#.0o" of zero still has to produce a leading zero
    if (spec->hash && base == 8 && isZero && digitCount == 0)
	reversed[digitCount++] = '0';

    //sign and alternative-form prefix, printed before any zero padding
    char prefix[3];
    int prefixLen = 0;

    if (negative)
	prefix[prefixLen++] = '-';
    else if (spec->plus)
	prefix[prefixLen++] = '+';
    else if (spec->space)
	prefix[prefixLen++] = ' ';

    if (spec->hash && !isZero)
    {
	if (base == 16)
	{
	    prefix[prefixLen++] = '0';
	    prefix[prefixLen++] = upper ? 'X' : 'x';
	}
	else if (base == 2)
	{
	    prefix[prefixLen++] = '0';
	    prefix[prefixLen++] = upper ? 'B' : 'b';
	}
    }

    int zeros = spec->prec > digitCount ? spec->prec - digitCount : 0;

    //'#' for octal increases the precision just enough to force a leading
    //zero, so it is not an extra prefix on top of the precision zeros
    if (spec->hash && base == 8 && !isZero && zeros == 0)
	zeros = 1;
    int total = prefixLen + zeros + digitCount;
    int pad = spec->width > total ? spec->width - total : 0;

    //'0' is ignored when left aligning or when a precision is given
    bool zeroPad = spec->zero && !spec->minus && spec->prec < 0;

    if (!spec->minus && !zeroPad)
	SinkRepeat(out, ' ', pad);

    SinkWrite(out, prefix, (size_t)prefixLen);
    SinkRepeat(out, '0', zeros);

    if (zeroPad)
	SinkRepeat(out, '0', pad);

    while (digitCount > 0)
	SinkChar(out, reversed[--digitCount]);

    if (spec->minus)
	SinkRepeat(out, ' ', pad);
}

static void EmitString(Sink *out, const char *s, const Spec *spec)
{
    if (!s)
	s = "(null)";

    size_t len = 0;
    while (s[len] != '\0' && (spec->prec < 0 || len < (size_t)spec->prec))
	len++;

    EmitText(out, s, len, spec);
}

static uint64_t FetchUnsigned(va_list *args, Length len)
{
    switch (len)
    {
	case LEN_HH: return (unsigned char)va_arg(*args, unsigned int);
	case LEN_H:  return (unsigned short)va_arg(*args, unsigned int);
	case LEN_L:  return va_arg(*args, unsigned long);
	case LEN_LL: return va_arg(*args, unsigned long long);
	case LEN_J:  return va_arg(*args, uintmax_t);
	case LEN_Z:  return va_arg(*args, size_t);
	case LEN_T:  return (uint64_t)va_arg(*args, ptrdiff_t);
	default:     return va_arg(*args, unsigned int);
    }
}

static int64_t FetchSigned(va_list *args, Length len)
{
    switch (len)
    {
	case LEN_HH: return (signed char)va_arg(*args, int);
	case LEN_H:  return (short)va_arg(*args, int);
	case LEN_L:  return va_arg(*args, long);
	case LEN_LL: return va_arg(*args, long long);
	case LEN_J:  return va_arg(*args, intmax_t);
	case LEN_Z:  return (int32_t)va_arg(*args, size_t);
	case LEN_T:  return va_arg(*args, ptrdiff_t);
	default:     return va_arg(*args, int);
    }
}

static const char *ParseSpec(const char *p, Spec *spec, va_list *args)
{
    spec->minus = spec->plus = spec->space = spec->hash = spec->zero = false;
    spec->width = 0;
    spec->prec = -1;
    spec->len = LEN_NONE;

    for (;; p++)
    {
	if (*p == '-')		spec->minus = true;
	else if (*p == '+')	spec->plus = true;
	else if (*p == ' ')	spec->space = true;
	else if (*p == '#')	spec->hash = true;
	else if (*p == '0')	spec->zero = true;
	else			break;
    }

    if (*p == '*')
    {
	spec->width = va_arg(*args, int);
	p++;

	if (spec->width < 0)
	{
	    spec->minus = true;
	    spec->width = -spec->width;
	}
    }
    else
    {
	while (*p >= '0' && *p <= '9')
	    spec->width = spec->width * 10 + (*p++ - '0');
    }

    if (*p == '.')
    {
	p++;

	if (*p == '*')
	{
	    spec->prec = va_arg(*args, int);
	    p++;

	    if (spec->prec < 0)
		spec->prec = -1;
	}
	else
	{
	    spec->prec = 0;

	    while (*p >= '0' && *p <= '9')
		spec->prec = spec->prec * 10 + (*p++ - '0');
	}
    }

    switch (*p)
    {
	case 'h':
	    if (p[1] == 'h') { spec->len = LEN_HH; p += 2; }
	    else	     { spec->len = LEN_H;  p++;    }
	    break;
	case 'l':
	    if (p[1] == 'l') { spec->len = LEN_LL; p += 2; }
	    else	     { spec->len = LEN_L;  p++;    }
	    break;
	case 'j': spec->len = LEN_J; p++; break;
	case 'z': spec->len = LEN_Z; p++; break;
	case 't': spec->len = LEN_T; p++; break;
	default: break;
    }

    return p;
}

static size_t Vformat(Sink *out, const char *fmt, va_list *args)
{
    for (const char *p = fmt; *p != '\0'; p++)
    {
	if (*p != '%')
	{
	    SinkChar(out, *p);
	    continue;
	}

	Spec spec;
	p = ParseSpec(p + 1, &spec, args);

	if (*p == '\0')
	{
	    SinkChar(out, '%');
	    return out->written;
	}

	switch (*p)
	{
	    case 'd':
	    case 'i': {
		int64_t value = FetchSigned(args, spec.len);
		bool negative = value < 0;

		//(value + 1) keeps INT64_MIN from overflowing on negation
		uint64_t magnitude = negative
		    ? (uint64_t)(-(value + 1)) + 1
		    : (uint64_t)value;

		EmitNumber(out, magnitude, 10, false, negative, &spec);
		break;
	    }
	    case 'u':
		EmitNumber(out, FetchUnsigned(args, spec.len), 10, false, false, &spec);
		break;
	    case 'o':
		EmitNumber(out, FetchUnsigned(args, spec.len), 8, false, false, &spec);
		break;
	    case 'x':
		EmitNumber(out, FetchUnsigned(args, spec.len), 16, false, false, &spec);
		break;
	    case 'X':
		EmitNumber(out, FetchUnsigned(args, spec.len), 16, true, false, &spec);
		break;
	    case 'b':
		EmitNumber(out, FetchUnsigned(args, spec.len), 2, false, false, &spec);
		break;
	    case 'c': {
		char c = (char)va_arg(*args, int);
		EmitText(out, &c, 1, &spec);
		break;
	    }
	    case 's':
		EmitString(out, va_arg(*args, const char *), &spec);
		break;
	    case 'p': {
		void *ptr = va_arg(*args, void *);

		if (!ptr)
		    EmitString(out, "(nil)", &spec);
		else
		{
		    Spec ptrSpec = spec;
		    ptrSpec.hash = true;
		    EmitNumber(out, (uintptr_t)ptr, 16, false, false, &ptrSpec);
		}
		break;
	    }
	    case '%':
		SinkChar(out, '%');
		break;
	    default:
		SinkChar(out, '%');
		SinkChar(out, *p);
		break;
	}
    }

    return out->written;
}

static void VgaWrite(void *ctx, const char *s, size_t len)
{
    (void)ctx;

    for (size_t i = 0; i < len; i++)
	Putc(s[i]);
}

int vprintf(const char *restrict fmt, va_list args)
{
    Sink out = { VgaWrite, NULL, 0 };
    return (int)Vformat(&out, fmt, &args);
}

typedef struct
{
    char *buf;
    size_t cap;
    size_t len;		//bytes actually stored, excluding the terminator
    size_t total;	//bytes the full output would have needed
} BufferSink;

static void BufferWrite(void *ctx, const char *s, size_t len)
{
    BufferSink *buf = (BufferSink *)ctx;

    buf->total += len;

    if (buf->cap == 0)
	return;

    size_t room = buf->cap - 1 - buf->len;
    if (len > room)
	len = room;

    memcpy(buf->buf + buf->len, s, len);
    buf->len += len;
}

//formats into dst like the C library, at most cap - 1 chars plus a
//terminator, returns the length the full string would have had
int snprintf(char *restrict dst, size_t cap, const char *restrict fmt, ...)
{
    BufferSink buf = { dst, cap, 0, 0 };
    Sink out = { BufferWrite, &buf, 0 };

    va_list args;
    va_start(args, fmt);
    Vformat(&out, fmt, &args);
    va_end(args);

    if (cap > 0)
	dst[buf.len] = '\0';

    return (int)buf.total;
}

int printf(const char *restrict fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int written = vprintf(fmt, args);
    va_end(args);

    return written;
}

void PrintfAt(int x, int y, Color foreground, Color background, const char *fmt, ...)
{
    SetCurPos(x, y);
    SetColor(foreground, background);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
