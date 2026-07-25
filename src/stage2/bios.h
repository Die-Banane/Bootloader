#pragma once
#include <stdint.h>
#include <stdbool.h>

#define DAP_ADR 0x7C00

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
