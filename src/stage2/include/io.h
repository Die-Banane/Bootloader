#pragma once

#include <stddef.h>

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

//cursor and colors, all output funcs below print at the current cursor
//position using the current colors and leave the cursor right behind the
//last character they wrote
void ClearScreen(void); //clears the screen and resets the cursor to 0,0
void SetCurPos(int x, int y);
void SetColor(Color foreground, Color background);

void Puts(const char *msg);

//supports %[flags][width][.precision][length] with conversions
//d i u o x X b c s p %, flags - + space 0 #, length hh h l ll z t
void Printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

//same formatter as Printf, writes at most cap - 1 chars and a terminator,
//returns the length the untruncated output would have had
int Snprintf(char *dst, size_t cap, const char *fmt, ...)
    __attribute__((format(printf, 3, 4)));

//absolute positioned variants, a convenience wrapper around
//SetCurPos + SetColor + Printf
void PutsAt(int x, int y, Color foreground, Color background, const char *msg);
void PrintfAt(int x, int y, Color foreground, Color background, const char *fmt, ...)
    __attribute__((format(printf, 5, 6)));