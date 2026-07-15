#include <stdint.h>
#include "bios.h"


void LbaToSegOff(uint16_t *seg, uint16_t *off, int lba)
{
    *seg = lba / 16;
    *off = lba % 16;
}

bool ReadDisk(uint8_t drive, int lba, uint16_t count, uint64_t sector)
{
    static volatile Dap *DAP = (Dap*)DAP_ADR;

    uint16_t seg, off;
    LbaToSegOff(&seg, &off, lba);
    uint32_t dest = (seg << 16) | off;

    DAP->size 		= 16;
    DAP->reserved	= 0;
    DAP->count 		= count;
    DAP->dest 		= dest;
    DAP->sector 	= sector;

    return BiosReadDisk(drive, DAP);
}
