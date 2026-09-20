#pragma once
#include <stddef.h>
#include "io.h"

struct TableEnt
{
    uint8_t 	active;
    
    //start of partition
    uint8_t 	startingHead;
    uint16_t 	start; //bits 0-5: starting sector bits 6-15: starting cylinder

    uint8_t 	systemId;
    
    //end of partition
    uint8_t 	endingHead;
    uint16_t 	end; //bits 0-5: ending sector bits 6-15: ending cylinder

    uint32_t 	startLba;
    uint32_t 	totalSectors;
} __attribute__((packed));

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

typedef enum
{
    NOERR,
    NOACTIVEPART,
    NOBOOTSIG
} ErrorType;

typedef enum
{
    FAT_NOTSUPPORTED,
    FAT_UNKNOWN,
    FAT_12,
    FAT_16,
    FAT_32
} FatType;

struct TableEnt *GetActivePartition();
void FatInit(uint8_t drive);
FatType FatDetect(struct Bpb *bpb);
void Error();
