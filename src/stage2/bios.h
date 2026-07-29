#pragma once
#include <stdint.h>
#include <stdbool.h>

#define STAGE2_POS 0x7E00
#define DAP_ADR 0x7C00
#define RM_MAX 0xffff*0x10+0xffff //highest addressabile address in real mode
#define SECTOR_SIZE 512 //size of one sector

typedef struct 
{
    uint8_t	size;
    uint8_t	reserved;
    uint16_t	count;
    uint32_t	dest; //format: seg:off
    uint64_t	sector;
} __attribute__((packed)) Dap;

void* ReadDisk(uint8_t drive, int lba, uint16_t count, uint64_t sector);
bool __attribute__((cdecl)) BiosReadDisk(uint8_t drive, volatile Dap *dap);
