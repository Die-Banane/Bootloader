#include <io.h>
#include <stdbool.h>
#include <bios.h>
#include <memory.h>
#include <stdint.h>
#include "fat.h"
#include "filesystem.h"

static struct
{
    union DirSlot {
	struct DirEnt ent;
	struct longFile lfn;
    } *dir;

    uint32_t entries;
} rootDir = {0};

static const struct FatVolume *vol;

static int Toupper(int c)
{
    return (c >= 'a' && c <='z')
	? c + ('A' - 'a')
	: c;
}

static size_t Strlen(const char *str)
{
    size_t len = 0;

    for(; *str; len++, str++) continue;
    return len;
}

static int Strcmp(const char *s1, const char *s2)
{
    while (*s1 || *s2)
    {
	if (*s1 != *s2)
	{
	    return *s1 < *s2
		? -1
		: 1;
	}

	s1++;
	s2++;
    }

    return 0;
}

static bool LoadCluster(uint16_t cluster, uintptr_t location)
{
    uint64_t sector = ((cluster - 2) * vol->sectorsPerCluster) + vol->dataLba;
    if (!ReadDisk(vol->drive, vol->sectorsPerCluster, sector, location))
	return false;

    return true;
}


static bool To83(const char *str, char out[12])
{
    // length = name + extension + '.'
    size_t len = Strlen(str);
    if (len > 8 + 3 + 1) return false;

    size_t remaining;
    char *name = out;

    for (size_t i = 0; i < len; i++, name++)
    {
	if (str[i] != '.')
	{
	    *name = Toupper(str[i]);
	}
	else
	{
	    if (i > 8) return false;
	    remaining = 8 - i;
	    for (size_t j = 0; j < remaining; j++, name++)
	    {
		*name = ' ';
	    }
	    str += i + 1;
	    break;
	}
    }

    for (size_t i = 0; i < 3; i++, str++, name++)
    {
	if (*str == '\0')
	{
	    remaining = 3 - i;
	    for (size_t j = 0; j < remaining; j++, name++)
		*name = ' ';

	    break;
	}
	    
	*name = Toupper(*str);
    }

    *name = '\0';
    return true;
}

static struct DirEnt *GetEntry(const char *file, FsErr *err)
{
    if (!rootDir.dir)
    {
	vol = GetFatVol();
	rootDir.dir = ReadDisk(vol->drive, vol->rootSectors, vol->rootLba, ROOT_DIR_POS);
	if (!rootDir.dir)
	{
	    *err = FS_IO_ERROR;
	    return NULL;
	}

	rootDir.entries = vol->rootEntries;
    }

    char file83[12];
    char cFileName[12];
    cFileName[11] = '\0';

    To83(file, file83);

    for (size_t i = 0; i < rootDir.entries; i++)
    {
	struct DirEnt current = rootDir.dir[i].ent;
	if (current.attributes == LFN)
	    //TODO: read long file entries correctly
	    continue;

	Memcpy(cFileName, current.fileName, sizeof(cFileName) / sizeof(cFileName[0]) - 1);

	if (Strcmp(cFileName, file83) == 0)
	{
	    *err = FS_OK;
	    return &(rootDir.dir[i].ent);
	}
    }

    *err = FS_FILE_NOT_FOUND;
    return NULL;
}
static cluster_t FindNextCusterLBA(cluster_t currentCluster)
{
    //TODO load Fat first into RAM, then read it
    return ((uint16_t*)(vol->fatLba * vol->bytesPerSector))[currentCluster];
}

void *Fat16ReadFile(const char *file, size_t size, uintptr_t dst, FsErr *err)
{
    Printf("reading %zu bytes from %s...\n", size, file);
    if (size == 0) return NULL;

    struct DirEnt *ent = GetEntry(file, err);
    uint32_t bytesPerCluster = vol->sectorsPerCluster * vol->bytesPerSector;
    uint32_t fileSectors = ent->size / bytesPerCluster / vol->bytesPerSector;
    uint32_t remainingSectors = (ent->size % bytesPerCluster) / vol->bytesPerSector;
    uint32_t totalSectors = fileSectors + remainingSectors;

    cluster_t currentCluster = ent->firstClusterLow;
    uintptr_t dest = dst;
    uint32_t remaining = totalSectors;
    while (remaining)
    {	
	if (remaining >= vol->sectorsPerCluster)
	{
	    if (!LoadCluster(currentCluster, dest))
	    {
		*err = FS_IO_ERROR;
		return NULL;
	    }
	    dest += bytesPerCluster;
	    remaining -= vol->sectorsPerCluster;
	    currentCluster = FindNextCusterLBA(currentCluster);
	}
	else
	{
	    if (!ReadDisk(vol->drive, remaining, currentCluster, dest))
	    {
		*err = FS_IO_ERROR;
		return NULL;
	    }
	}
    }
    
    //int32_t currentCluster = ent->firstClusterLow;
    //uintptr_t dest = dst;
    //while (clusterNumber)
    //{
    	//if (!LoadCluster(currentCluster, dest))
	//{
	    //*err = FS_IO_ERROR;
	    //break;
	//}
	//dest += bytesPerCluster;
	//currentCluster = FindNextCluster(currentCluster);
    //}

    if (*err != FS_OK)
	return NULL;

    return (void*)dst;
}
