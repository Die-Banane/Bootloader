#include <stdint.h>

volatile uint16_t *vga = (uint16_t *)0xb8000;

void c_main(void)
{
    vga[1] = 0x0f42;

    for (;;)
	continue;
}
