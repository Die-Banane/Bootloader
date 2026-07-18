#include <stdint.h>
#include <stdarg.h>
#include "io.h"

volatile uint16_t *vga = (uint16_t*)0xb8000;

void Puts(int x, int y, Color foreground, Color background, const char *msg)
{
    if (x >= COLS || y >= ROWS) return;

    uint8_t c;
    
    uint16_t color = (background << 4) | foreground;

    int offset = COLS * y + x;
    
    for (int i = 0; (c = msg[i]) != '\0'; i++)
	vga[offset + i] = ((color << 8) | c);
}

void ClearScreen(void)
{
    for (int i = 0; i < ROWS * COLS; i++)
	vga[i] = 0;
}

static char *UintToStr(char *buf, unsigned int val, int base, int uppercase)
{
    char *p = buf;
    char *start = buf;
    char hex[] = "0123456789abcdef";
    char HEX[] = "0123456789ABCDEF";
    char *digits = uppercase ? HEX : hex;

    if (val == 0)
	*p++ = '0';

    while (val > 0) {
	*p++ = digits[val % base];
	val /= base;
    }

    *p = '\0';

    for (int i = 0; i < (p - start) / 2; i++) {
	char tmp = start[i];
	start[i] = start[p - start - 1 - i];
	start[p - start - 1 - i] = tmp;
    }

    return p;
}

void Printf(int x, int y, Color foreground, Color background, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buf[256];
    int pos = 0;

    for (int i = 0; fmt[i] != '\0' && pos < (int)sizeof(buf) - 1; i++) {
	if (fmt[i] != '%') {
	    buf[pos++] = fmt[i];
	    continue;
	}

	i++;
	switch (fmt[i]) {
	    case 'd':
	    case 'i': {
		int val = va_arg(args, int);
		char tmp[16];
		if (val < 0) {
		    buf[pos++] = '-';
		    val = -val;
		}
		UintToStr(tmp, (unsigned int)val, 10, 0);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case 'u': {
		unsigned int val = va_arg(args, unsigned int);
		char tmp[16];
		UintToStr(tmp, val, 10, 0);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case 'x': {
		unsigned int val = va_arg(args, unsigned int);
		char tmp[16];
		UintToStr(tmp, val, 16, 0);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case 'X': {
		unsigned int val = va_arg(args, unsigned int);
		char tmp[16];
		UintToStr(tmp, val, 16, 1);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case 's': {
		const char *s = va_arg(args, const char *);
		if (!s) s = "(null)";
		while (*s && pos < (int)sizeof(buf) - 1)
		    buf[pos++] = *s++;
		break;
	    }
	    case 'c': {
		char c = (char)va_arg(args, int);
		buf[pos++] = c;
		break;
	    }
	    case 'p': {
		void *ptr = va_arg(args, void *);
		char tmp[16];
		tmp[0] = '0';
		tmp[1] = 'x';
		UintToStr(tmp + 2, (unsigned int)ptr, 16, 0);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case '%':
		buf[pos++] = '%';
		break;
	    default:
		buf[pos++] = '%';
		if (fmt[i] && pos < (int)sizeof(buf) - 1)
		    buf[pos++] = fmt[i];
		break;
	}
    }

    buf[pos] = '\0';
    va_end(args);

    Puts(x, y, foreground, background, buf);
}
