#include <stdint.h>
#include <stddef.h>
#include "bios.h"

static void LinearToSegOff(uint16_t *seg, uint16_t *off, int destLba)
{
    if (destLba / 16 > 0xffff)
    {
	*seg = 0xffff;
	*off = destLba - (*seg * 0x10);
    }
    else
    {
	*seg = destLba / 16;
	*off = destLba % 16;
    }	
}

void* ReadDisk(uint8_t drive, uint16_t count, uint64_t sector, int destLba)
{
    if (destLba + count * SECTOR_SIZE > RM_MAX) return NULL;

    //IMPORTANT: NEVER PLACE DAP IN HIGH MEMORY 
    //BiosReadDisk WILL FAIL!!!!!
    static volatile Dap *DAP = (Dap*)DAP_ADR;

    uint16_t seg, off;
    LinearToSegOff(&seg, &off, destLba);
    uint32_t dest = (seg << 16) | off;

    DAP->size 		= 16;
    DAP->reserved	= 0;
    DAP->count 		= count;
    DAP->dest 		= dest;
    DAP->sector 	= sector;

    if (BiosReadDisk(drive, DAP))
        return (void*)(uintptr_t)destLba;
    else
        return NULL;
}
