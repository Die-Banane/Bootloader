#include <filesystem.h>
#include <fat.h>
#include <bios.h>
#include <partition.h>

FsErr FatInit(uint8_t drive, FatType *type)
{
    *type = FAT_UNKNOWN;

    struct TableEnt *activeEnt = GetActivePartition();
    if (!activeEnt)
	return FS_NO_PARTITION;

    struct Bpb *bpb = ReadDisk(drive, 1, activeEnt->startLba, 0x500);
    if (!bpb || bpb->bootSignature != 9)
	return FS_NO_BOOT_SIGNATURE;

    *type = FatDetect(bpb);
    switch (*type)
    {
	case FAT_UNKNOWN:
	    return FS_NO_FILESYSTEM;
	case FAT_NOT_SUPPORTED:
	    return FS_NOT_SUPPORTED;
	default:
	    return FS_OK;
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
