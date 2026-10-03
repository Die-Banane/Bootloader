#include "filesystem.h"
#include <stdint.h>
#include <fat.h>
#include <bios.h>
#include <partition.h>


Fat FatInit(uint8_t drive, char **errMsg)
{
    struct TableEnt *activeEnt = GetActivePartition();
    if (!activeEnt)
    {
	*errMsg = "error: fat: could not find an active partition on boot device";
	return (Fat){ .type = FAT_UNKNOWN, .err = FS_NO_PARTITION };
    }

    struct Bpb *bpb = ReadDisk(drive, 1, activeEnt->startLba, 0x500);
    if (!bpb || bpb->bootSignature != BOOT_SIG)
    {
	*errMsg = "error: fat: vbr does not contain a valid boot signature";
	return (Fat){.type = FAT_UNKNOWN, .err = FS_NO_FILESYSTEM };
    }

    FatType type = FatDetect(bpb);
    switch (type)
    {
	case FAT_UNKNOWN:
	    *errMsg = "err: fat: could not locate any FAT file system on active partition";
	    return (Fat){ .type = FAT_UNKNOWN, .err = FS_NO_FILESYSTEM };
	case FAT_NOT_SUPPORTED:
	    *errMsg = "err: fat: active partition does not contain a supported FAT file system";
	    return (Fat){ .type = FAT_NOT_SUPPORTED, .err = FS_NOT_SUPPORTED };
	default:
	    *errMsg = "";
	    return (Fat){ .type = type, .err = FS_OK };
    }
}

FatType FatDetect(struct Bpb *bpb)
{
    //exFat is not supported
    if (bpb->bytesPerSector == 0)
	return FAT_NOT_SUPPORTED;

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
