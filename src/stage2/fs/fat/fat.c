#include <filesystem.h>
#include <bios.h>
#include <partition.h>
#include "fat.h"

static struct FatVolume fatVol;

static FatType FatDetect(void)
{
    if (fatVol.totalClusters < 4085)
	return FAT_12;
    else if (fatVol.totalClusters < 65525)
	return FAT_16;
    else
	return FAT_32;
}

void FatInit(uint8_t drive, struct Fs *fs, FsErr *err)
{
    struct TableEnt *activeEnt = GetActivePartition();
    if (!activeEnt)
    {
	*err = FS_NO_PARTITION;
	return;
    }

    uint32_t partitionStart = activeEnt->startLba;

    struct Bpb *bpb = ReadDisk(drive, 1, partitionStart, 0x500);
    if (!bpb || bpb->bootSignature != BOOT_SIG)
    {
	*err = FS_NO_BOOT_SIGNATURE;
	return;
    }

    uint16_t bps = bpb->bytesPerSector;
    uint8_t scale = bps / SECTOR_SIZE;

    //exFat is not supported; bps must be a power of two >= 512
    if (bps >= 512 && (bps & (bps - 1)) == 0 && bpb->sectorsPerCluster != 0)
    {
	uint32_t totalSectors = bpb->totalSectors16 == 0
	    ? bpb->totalSectors32
	    : bpb->totalSectors16;

	uint32_t fatSectors = bpb->fatCount * (bpb->sectorsPerFat16 == 0
	    ? bpb->ext.ext32.sectorsPerFat32
	    : bpb->sectorsPerFat16);

	uint32_t rootSectors = ((uint32_t)bpb->rootEntriesCount * 32 + (bps - 1)) / bps;

	uint32_t dataSectors = totalSectors - (fatSectors + rootSectors + bpb->reservedSectors);

	fatVol.totalClusters = dataSectors / bpb->sectorsPerCluster;

	//translate everything to bios sectors
	fatVol.totalSectors = totalSectors * scale;
	fatVol.fatSectors = fatSectors * scale;
	fatVol.rootSectors = rootSectors * scale;
	fatVol.dataSectors = dataSectors * scale;

	fatVol.type = FatDetect();
    }
    else
	fatVol.type = FAT_UNKNOWN;

    fatVol.reserved = bpb->reservedSectors * scale;
    fatVol.bytesPerSector = bpb->bytesPerSector;
    fatVol.sectorsPerCluster = bpb->sectorsPerCluster * scale;

    fatVol.fatLba = partitionStart + fatVol.reserved;
    fatVol.rootLba = fatVol.fatLba + fatVol.fatSectors;
    fatVol.dataLba = fatVol.rootLba + fatVol.rootSectors;

    fatVol.drive = drive;

    fatVol.rootCluster = fatVol.type == FAT_32
	? bpb->ext.ext32.rootCluster
	: 0;

    fatVol.rootEntries = bpb->rootEntriesCount;

    switch (fatVol.type)
    {
	case FAT_UNKNOWN:
	    fs->ReadFile = NULL;
	    *err = FS_NO_FILESYSTEM;
	    return;

	case FAT_16:
	    fs->ReadFile = Fat16ReadFile;
	    *err = FS_OK;
	    return;

	default:
	    fs->ReadFile = NULL;
    }
}

const struct FatVolume *GetFatVol(void)
{
    return &fatVol;
}
