#include <stddef.h>
#include "fat.h"
#include "bios.h"
#include "io.h"

static struct TableEnt *table = (struct TableEnt*)(0x7C00 + 0x01BE);

static ErrorType errCode = NOERR;

void FatInit(uint8_t drive)
{
    struct TableEnt *activeEnt;
    if (!(activeEnt = GetActivePartition()))
    {
	errCode = NOACTIVEPART;
	goto err;
    }

    struct Bpb *bpb = ReadDisk(drive, 1, activeEnt->startLba, 0x500);

    if (bpb->bootSignature != 0xAA55)
    {
	errCode = NOBOOTSIG;
	goto err;
    }

    FatType fatType = FatDetect(bpb);

    ClearScreen();
    switch (fatType)
    {
	case FAT_UNKNOWN:
	    Printf(0, 0, LIGHT_RED, BLACK, "error: could not detect a FAT filesystem on active partition");
	    break;

	case FAT_12:
	    Printf(0, 0, WHITE, BLACK, "FAT 12 detected");
	    break;

	case FAT_16:
	    Printf(0, 0, WHITE, BLACK, "FAT 16 detected");
	    break;

	case FAT_32:
	    Printf(0, 0, WHITE, BLACK, "FAT 32 detected");
	    break;

	case FAT_NOTSUPPORTED:
	    Printf(0, 0, LIGHT_RED, BLACK, "unsupported fat filesystem");
	    break;

	default:
	    Printf(0, 0, LIGHT_RED, BLACK, "default");
	    break;
    }

err:
    if (errCode != NOERR) Error();
    return;
}

struct TableEnt *GetActivePartition()
{
    struct TableEnt *current = table;
    
    for (int i = 0; i < 4; i++, current++)
	if (current->active == 0x80) return current;

    return NULL;
}

FatType FatDetect(struct Bpb *bpb)
{
    //exFat is not supported
    if (bpb->bytesPerSector == 0) return FAT_NOTSUPPORTED;

    FatType type = FAT_UNKNOWN;

    uint32_t totalSectors = bpb->totalSectors16 == 0
	? bpb->totalSectors32
	: bpb->totalSectors16;
    
    uint32_t fatSectors = bpb->fatCount * (bpb->sectorsPerFat16 == 0
	? bpb->ext.ext32.sectorsPerFat32
	: bpb->sectorsPerFat16);

    uint32_t rootSectors = ((uint32_t)bpb->rootEntriesCount * 32 + (bpb->bytesPerSector - 1)) / bpb->bytesPerSector;

    uint32_t dataSectors = totalSectors - (fatSectors + rootSectors + bpb->reservedSectors);
    uint32_t totalClusters = dataSectors / bpb->sectorsPerCluster;

    if (totalClusters < 4085)
	type = FAT_12;
    else if (totalClusters < 65525)
	type = FAT_16;
    else
	type = FAT_32;

    return type;
}

void Error()
{
    ClearScreen();

    switch (errCode)
    {
	case NOACTIVEPART:
	    Printf(0, 0, LIGHT_RED, BLACK, "fat: error: could not find active partition");
	    break;

	case NOBOOTSIG:
	    Printf(0, 0, LIGHT_RED, BLACK, "fat: error: active Partition does not contain a boot signature");
	    break;

	default:
	    Printf(0, 0, LIGHT_RED, BLACK, "fat: error: an error occured while trying to initialize the fat driver");
	    break;
    }
}
