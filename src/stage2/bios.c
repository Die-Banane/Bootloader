#include <stdint.h>
#include "bios.h"
#include "io.h"


static void LinearToSegOff(uint16_t *seg, uint16_t *off, int lba)
{
    *seg = lba / 16;
    *off = lba % 16;
}

void* ReadDisk(uint8_t drive, int linear, uint16_t count, uint64_t sector)
{
    //IMPORTANT: NEVER PLACE DAP IN HIGH MEMORY 
    //BiosReadDisk WILL FAIL!!!!!
    static volatile Dap *DAP = (Dap*)DAP_ADR;

    uint16_t seg, off;
    LinearToSegOff(&seg, &off, linear);
    uint32_t dest = (seg << 16) | off;

    DAP->size 		= 16;
    DAP->reserved	= 0;
    DAP->count 		= count;
    DAP->dest 		= dest;
    DAP->sector 	= sector;

    if (BiosReadDisk(drive, DAP))
        return (void*)(uintptr_t)linear;
    else
        return NULL;
}
