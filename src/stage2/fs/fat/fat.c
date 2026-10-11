#include <filesystem.h>
#include <fat.h>
#include <bios.h>
#include <partition.h>

static FatType FatDetect(struct Bpb *bpb)
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

void FatInit(uint8_t drive, struct Fs *fs, FsErr *err)
{
    struct TableEnt *activeEnt = GetActivePartition();
    if (!activeEnt)
    {
	*err = FS_NO_PARTITION;
	return;
    }

    struct Bpb *bpb = ReadDisk(drive, 1, activeEnt->startLba, 0x500);
    if (!bpb || bpb->bootSignature != BOOT_SIG)
    {
	*err = FS_NO_BOOT_SIGNATURE;
	return;
    }

    FatType type = FatDetect(bpb);
    switch (type)
    {
	case FAT_UNKNOWN:
	    fs->ReadFile = NULL;
	    *err = FS_NO_FILESYSTEM;
	    return;

	case FAT_NOT_SUPPORTED:
	    fs->ReadFile = NULL;
	    *err = FS_NOT_SUPPORTED;
	    return;

	case FAT_12:
	    fs->ReadFile = Fat12ReadFile;
	    return;

	case FAT_16:
	    fs->ReadFile = Fat16ReadFile;
	    return;

	case FAT_32:
	    fs->ReadFile = Fat32ReadFile;
	    return;
    }
}

