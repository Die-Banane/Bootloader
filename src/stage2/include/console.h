/*
 * this file is almost entirely vibe coded
 */
#ifndef CONSOLE_H
#define CONSOLE_H

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

void Putc(char c);
void Puts(const char *msg);

//absolute positioned variants, a convenience wrapper around
//SetCurPos + SetColor + printf
void PutsAt(int x, int y, Color foreground, Color background, const char *msg);
void PrintfAt(int x, int y, Color foreground, Color background, const char *fmt, ...)
    __attribute__((format(printf, 5, 6)));
    
#endif
