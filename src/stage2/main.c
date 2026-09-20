#include <stddef.h>
#include "io.h"
#include "fat.h"

void __attribute__((cdecl)) c_main(uint32_t size, uint16_t space, uint8_t boot_drive)
{
    int ebdaStart = space * 1024;
    struct TableEnt *ent = GetActivePartition();

    ClearScreen();
    Printf(0, 0, WHITE, BLACK, "booted from drive: %x", boot_drive);
    Printf(0, 1, WHITE, BLACK, "stage 2 is %d bytes", size);
    Printf(0, 2, WHITE, BLACK, "EBDA starts at %x, space left is: ~%dKiB", ebdaStart, space);

    if (ent)
	Printf(0, 3, WHITE, BLACK, "Active Partition starts at %x", ent->startLba);
    else
	Printf(0, 3, LIGHT_RED, BLACK, "no active partition found");

    FatInit(boot_drive);

err:
    for (;;)
	continue;
}
