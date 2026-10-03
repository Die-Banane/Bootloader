#pragma once
#include <stddef.h>

#define STAGE2_POS 0x7E00
#define DAP_ADR 0x7C00
#define RM_MAX (0xffff*0x10+0xffff) //highest addressabile address in real mode
#define SECTOR_SIZE 512 //size of one sector in bytes
#define VGA_POS 0xB8000

void *Memcpy(void *dest, void *src, size_t size);
