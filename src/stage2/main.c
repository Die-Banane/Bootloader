#include <stdint.h>
#include "bios.h"
#include "io.h"

void __attribute__((cdecl)) c_main(uint32_t size, uint8_t boot_drive)
{
    ClearScreen();
    Printf(0, 0, BLACK, WHITE, "booted from drive: %x size of the loader is: %x", boot_drive, size);

    int linear		= 0x10000;
    uint32_t count	= 1;
    uint64_t sector	= 0;

    uint8_t *p = (uint8_t*)ReadDisk(boot_drive, linear, count, sector);

    if (!p)
    {
        Puts(0, 2, RED, WHITE, "disk read failed");
	goto err;
    }

    if (p[510] == 0x55 && p[511] == 0xAA)
	Puts(0, 1, GREEN, WHITE, "klasse");
    else
	Puts(0, 1, RED, WHITE, "doof");

err:
    for (;;)
	continue;
}
