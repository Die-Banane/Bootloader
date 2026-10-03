#include <stddef.h>
#include <partition.h>

static struct Mbr *mbr = (struct Mbr*)(MBR_POS);

struct TableEnt *GetActivePartition()
{
    for (int i = 0; i < 4; i++)
    {
	if (mbr->table[i].active == 0x80) 
	    return &mbr->table[i];
    }

    return NULL;
}
