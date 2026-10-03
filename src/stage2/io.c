#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <memory.h>
#include <io.h>

volatile uint16_t *vga = (uint16_t*)VGA_POS;

static int cursor_x;
static int cursor_y;
static Color foreground = WHITE;
static Color background = BLACK;

static uint16_t ColorAttr(Color fg, Color bg)
{
    return (uint16_t)(((bg << 4) | fg) << 8);
}

static void BlankRow(int y)
{
    uint16_t attr = ColorAttr(background, background);

    for (int x = 0; x < COLS; x++)
	vga[COLS * y + x] = attr | ' ';
}

//shifts the whole screen up by one row, the caller is responsible for
//leaving the cursor in the last row
static void ScrollUp(void)
{
    for (int i = 0; i < (ROWS - 1) * COLS; i++)
	vga[i] = vga[i + COLS];

    BlankRow(ROWS - 1);
}

static void Putc(char c)
{
    switch (c)
    {
	case '\n':
	    cursor_x = 0;
	    cursor_y++;
	    break;
	case '\r':
	    cursor_x = 0;
	    break;
	case '\t':
	    cursor_x = (cursor_x + 8) & ~7;
	    break;
	default:
	    vga[COLS * cursor_y + cursor_x] = ColorAttr(foreground, background) | c;
	    cursor_x++;
	    break;
    }

    //wrap around at the right margin
    if (cursor_x >= COLS)
    {
	cursor_x = 0;
	cursor_y++;
    }

    if (cursor_y >= ROWS)
    {
	cursor_y = ROWS - 1;
	ScrollUp();
    }
}

void SetCurPos(int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= COLS) x = COLS - 1;
    if (y >= ROWS) y = ROWS - 1;

    cursor_x = x;
    cursor_y = y;
}

void SetColor(Color fg, Color bg)
{
    foreground = fg;
    background = bg;
}

void ClearScreen(void)
{
    for (int y = 0; y < ROWS; y++)
	BlankRow(y);

    cursor_x = 0;
    cursor_y = 0;
}

void Puts(const char *msg)
{
    for (int i = 0; msg[i] != '\0'; i++)
	Putc(msg[i]);
}

void PutsAt(int x, int y, Color foreground, Color background, const char *msg)
{
    SetCurPos(x, y);
    SetColor(foreground, background);
    Puts(msg);
}

static void UintToStr(char *buf, unsigned int val, int base)
{
    char *p = buf;
    char *start = buf;
    char digits[] = "0123456789abcdef";

    if (val == 0)
	*p++ = '0';

    while (val > 0)
    {
	*p++ = digits[val % base];
	val /= base;
    }

    *p = '\0';

    for (int i = 0; i < (p - start) / 2; i++)
    {
	char tmp = start[i];
	start[i] = start[p - start - 1 - i];
	start[p - start - 1 - i] = tmp;
    }
}

static void Emit(const char *buf, int len)
{
    for (int i = 0; i < len; i++)
	Putc(buf[i]);
}

static void Vprintf(const char *fmt, va_list args)
{
    char buf[256];
    int pos = 0;

    for (int i = 0; fmt[i] != '\0' && pos < (int)sizeof(buf) - 1; i++) 
    {
	if (fmt[i] != '%')
	{
	    buf[pos++] = fmt[i];
	    continue;
	}

	i++;
	switch (fmt[i])
	{
	    case 'd':
	    case 'i': {
		int val = va_arg(args, int);
		char tmp[16];
		if (val < 0)
		{
		    if (pos < (int)sizeof(buf) - 1)
			buf[pos++] = '-';
		    val = -val;
		}
		UintToStr(tmp, (unsigned)val, 10);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case 'u': {
		size_t val = va_arg(args, unsigned int);
		char tmp[16];
		UintToStr(tmp, val, 10);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case 'X':
	    case 'x': {
		unsigned int val = va_arg(args, unsigned int);
		char tmp[16];
		tmp[0] = '0';
		tmp[1] = 'x';
		UintToStr(tmp + 2, val, 16);
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
		if (pos < (int)sizeof(buf) - 1)
		    buf[pos++] = c;
		break;
	    }
	    case 'p': {
		void *ptr = va_arg(args, void *);
		char tmp[16];
		tmp[0] = '0';
		tmp[1] = 'x';
		UintToStr(tmp + 2, (uintptr_t)ptr, 16);
		for (char *s = tmp; *s && pos < (int)sizeof(buf) - 1; s++)
		    buf[pos++] = *s;
		break;
	    }
	    case '%':
		if (pos < (int)sizeof(buf) - 1)
		    buf[pos++] = '%';
		break;
	    default:
		if (pos < (int)sizeof(buf) - 1)
		    buf[pos++] = '%';
		if (fmt[i] && pos < (int)sizeof(buf) - 1)
		    buf[pos++] = fmt[i];
		break;
	}
    }

    Emit(buf, pos);
}

void Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Vprintf(fmt, args);
    va_end(args);
}

void PrintfAt(int x, int y, Color foreground, Color background, const char *fmt, ...)
{
    SetCurPos(x, y);
    SetColor(foreground, background);

    va_list args;
    va_start(args, fmt);
    Vprintf(fmt, args);
    va_end(args);
}
