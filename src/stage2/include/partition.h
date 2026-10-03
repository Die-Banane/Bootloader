#pragma once
#include <stdint.h>

#define MBR_POS 0x7C00
//0x55 0xAA on disk; read back as a little-endian uint16_t this is 0xAA55
#define BOOT_SIG 0xAA55

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

struct Mbr
{
    uint8_t		raw[440];

    //optional fields
    uint32_t		diskId;
    uint16_t		reserved;

    struct TableEnt	table[4];

    uint16_t		bootSignature;
} __attribute__((packed));

struct TableEnt *GetActivePartition();
