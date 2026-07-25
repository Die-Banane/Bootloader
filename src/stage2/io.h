#pragma once
#include <stdint.h>
#include <stdarg.h>

#define NULL 0

#define ROWS 25
#define COLS 80

typedef enum {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GREY,
    DARK_GREY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    YELLOW,
    WHITE
} Color;

void Puts(int x, int y, Color foreground, Color background, const char *msg);
void ClearScreen(void);
void Printf(int x, int y, Color foreground, Color background, const char *fmt, ...);
