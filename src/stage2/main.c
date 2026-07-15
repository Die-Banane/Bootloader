#include <stdint.h>
#include "bios.h"
#include "io.h"



void __attribute__((cdecl)) c_main(uint16_t boot_drive)
{
    ReadDisk(boot_drive, 0x10000, 1, 0);

    uint8_t *p = (uint8_t*)0x10000;

    if (p[510] == 0x55 && p[511] == 0xAA)
	Puts(0, 0, GREEN, MAGENTA, "klasse");
    else
	Puts(0, 0, GREEN, MAGENTA, "doof");

    for (;;)
	continue;
}
