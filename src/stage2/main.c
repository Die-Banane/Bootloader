#include <stdint.h>
#include "bios.h"
#include "io.h"

void __attribute__((cdecl)) c_main(uint16_t boot_drive)
{
    ClearScreen();
    Printf(0, 0, BLACK, WHITE, "booted from drive %x", boot_drive);

    int dest		= 0x10000;
    uint32_t count	= 1;
    uint64_t sector	= 0;

    ReadDisk(boot_drive, dest, count, sector);

    uint8_t *p = (uint8_t*)0x10000;
    
    if (p[510] == 0x55 && p[511] == 0xAA)
	Puts(0, 1, GREEN, WHITE, "klasse");
    else
	Puts(0, 1, RED, WHITE, "doof");

    for (;;)
	continue;
}
