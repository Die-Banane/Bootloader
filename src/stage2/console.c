/*
 * this file is almost entirely vibe coded
 */
#include <stdint.h>
#include <memory.h>
#include <console.h>

static volatile uint16_t *vga = (uint16_t*)VGA_POS;

static int cursorX;
static int cursorY;
static uint16_t color = ((BLACK << 4) | WHITE) << 8;

static void BlankRow(int y)
{
    for (int x = 0; x < COLS; x++)
	vga[COLS * y + x] = color | ' ';
}

//shifts the whole screen up by one row, the caller is responsible for
//leaving the cursor in the last row
static void ScrollUp(void)
{
    for (int i = 0; i < (ROWS - 1) * COLS; i++)
	vga[i] = vga[i + COLS];

    BlankRow(ROWS - 1);
}

void Putc(char c)
{
    switch (c)
    {
	case '\n':
	    cursorX = 0;
	    cursorY++;
	    break;
	case '\r':
	    cursorX = 0;
	    break;
	case '\t':
	    cursorX= (cursorX + 8) & ~7;
	    break;
	default:
	    vga[COLS * cursorY + cursorX] = color | c;
	    cursorX++;
	    break;
    }

    //wrap around at the right margin
    if (cursorX >= COLS)
    {
	cursorX = 0;
	cursorY++;
    }

    if (cursorY >= ROWS)
    {
	cursorY = ROWS - 1;
	ScrollUp();
    }
}

void SetCurPos(int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= COLS) x = COLS - 1;
    if (y >= ROWS) y = ROWS - 1;

    cursorX = x;
    cursorY = y;
}

void SetColor(Color fg, Color bg)
{
    color = (uint16_t)(((bg << 4) | fg) << 8);
}

void ClearScreen(void)
{
    for (int y = 0; y < ROWS; y++)
	BlankRow(y);

    cursorX = 0;
    cursorY = 0;
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
