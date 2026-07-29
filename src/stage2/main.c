#include <stdint.h>
#include "bios.h"
#include "io.h"

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t boot_drive)
{
    int ebdaStart = space * 1024;
    int spaceLeft = ebdaStart - (STAGE2_POS + size); //space left in bytes

    ClearScreen();
    Printf(0, 0, WHITE, BLACK, "booted from drive: %x", boot_drive);
    Printf(0, 1, WHITE, BLACK, "size of the loader is: %x", size);
    Printf(0, 2, WHITE, BLACK, "EBDA starts at %x, space left is: ~%dk", ebdaStart, spaceLeft / 1024);

    int linear		= 0x10000;
    uint32_t count	= 1;
    uint64_t sector	= 0;

    uint8_t *p = (uint8_t*)ReadDisk(boot_drive, linear, count, sector);

    if (!p)
    {
        Puts(0, 3, RED, WHITE, "disk read failed");
	goto err;
    }

    if (p[510] == 0x55 && p[511] == 0xAA)
	Puts(0, 3, GREEN, WHITE, "klasse");
    else
	Puts(0, 3, RED, WHITE, "doof");

err:
    for (;;)
	continue;
}
