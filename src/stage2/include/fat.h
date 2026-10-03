#pragma once
#include <stdint.h>
#include <filesystem.h>

struct Ext16
{
    uint8_t 	driveNumber;
    uint8_t	flags;
    uint8_t	signature;
    uint32_t	volumeId;

    char	volumeLabel[11];
    char	systemIdentifier[8];

    uint8_t	raw[448];
} __attribute__((packed));

struct Ext32
{
    uint32_t	sectorsPerFat32;
    uint16_t	flags;
    uint16_t	fatVersion;
    uint32_t	rootCluster;
    uint16_t	fsInfoSector;
    uint16_t	backupBootSector;
    
    uint8_t	reserved[12];

    uint8_t	driveNumber;
    uint8_t	ntFlags;
    uint8_t	signature;
    uint32_t	volumeId;

    char	volumeLabel[11];
    char	systemIdentifier[8];

    uint8_t	raw[420];
} __attribute__((packed));

struct Bpb
{
    uint8_t 	jmp[3];
    char 	oemName[8];

    uint16_t 	bytesPerSector;
    uint8_t 	sectorsPerCluster;
    uint16_t 	reservedSectors;
    uint8_t 	fatCount;
    uint16_t 	rootEntriesCount;
    uint16_t 	totalSectors16;
    uint8_t 	mediadescriptor;
    uint16_t 	sectorsPerFat16;
    uint16_t 	sectorsPerTrack;
    uint16_t 	headCount;
    uint32_t 	hiddenSectors;
    uint32_t 	totalSectors32;

    union {
	struct Ext16 ext16;
	struct Ext32 ext32;
    } ext;

    uint16_t	bootSignature;
} __attribute__((packed));

struct DirEnt
{
    char	fileName[11];
    uint8_t	attributes;
    uint8_t	reserved;
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
    char	firstChars[5];
    uint8_t	attribute; //always 0x0F
    uint8_t	type;
    uint8_t	checksum;
    char	middleChars[6];
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
    FAT_NOT_SUPPORTED,
    FAT_UNKNOWN,
    FAT_12,
    FAT_16,
    FAT_32
} FatType;

typedef struct Fat
{
    FatType type;
    FsErr err;
} Fat;

Fat FatInit(uint8_t drive, char **errMsg);
FatType FatDetect(struct Bpb *bpb);
