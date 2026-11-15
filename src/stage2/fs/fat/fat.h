#ifndef FAT_H
#define FAT_H

#include <stddef.h>
#include <stdint.h>
#include <filesystem.h>

//behind the VBR wich was loaded into memory by Fatinit
#define ROOT_DIR_POS 0x700

typedef uint16_t cluster_t;

struct DirEnt
{
    char	fileName[11];
    uint8_t	attributes;
    uint8_t	reserved;
    uint8_t	creationTimeTenth;
    uint16_t	creationTime;
    uint16_t	creationDate;
    uint16_t	lastAccessDate;
    uint16_t	firstClusterHigh; //for FAT 12 and FAT 16 always 0
    uint16_t	lastModTime;
    uint16_t	lastModDate;
    uint16_t	firstClusterLow;
    uint32_t	size;
} __attribute__((packed));

struct longFile
{
    uint8_t	order;
    char	firstChars[10];
    uint8_t	attribute; //always 0x0F
    uint8_t	type;
    uint8_t	checksum;
    char	middleChars[12];
    uint16_t	zero; //always 0
    char	finalChars[4];
} __attribute__((packed));


typedef enum
{
    READ_ONLY	= 0x01,
    HIDDEN	= 0x02,
    SYSTEM	= 0x04,
    VOLUME_ID	= 0x08,
    DIRECTORY	= 0x10,
    ARCHIVE	= 0x20,
    LFN		= READ_ONLY | HIDDEN | SYSTEM | VOLUME_ID
} FAT_ATTRIBUTES;

typedef enum
{
    FAT_UNKNOWN = 0,
    FAT_12,
    FAT_16,
    FAT_32
} FatType;

struct FatVolume
{
    uint8_t drive;
    FatType type;

    uint16_t reserved;
    uint32_t fatSectors;
    uint32_t rootSectors;
    uint32_t dataSectors;

    uint16_t bytesPerSector;
    uint8_t  sectorsPerCluster;
    uint32_t totalSectors;
    uint32_t totalClusters;

    uint32_t fatLba;
    uint32_t dataLba;
    uint32_t rootLba;
    uint16_t rootEntries;

    //fat32 only
    uint32_t rootCluster;
};

void FatInit(uint8_t drive, struct Fs *fs, FsErr *err);
const struct FatVolume *GetFatVol(void);

void *Fat12ReadFile(const char *path, size_t size, uintptr_t dst, FsErr *err);
void *Fat16ReadFile(const char *path, size_t size, uintptr_t dst, FsErr *err);
void *Fat32ReadFile(const char *path, size_t size, uintptr_t dst, FsErr *err);

#endif
