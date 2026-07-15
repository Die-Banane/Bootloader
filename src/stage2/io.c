#include <stdint.h>
#include "io.h"

volatile uint16_t *vga = (uint16_t*)0xb8000;

void Puts(int x, int y, uint8_t foreground, uint8_t background, const char *msg)
{
    uint8_t c;
    
    uint16_t color = (foreground << 4) | background;

    int offset = y * x + x;
    
    for (int i = 0; (c = msg[i]) != '\0'; i++)
	vga[offset + i] = ((color << 8) | c);
}
