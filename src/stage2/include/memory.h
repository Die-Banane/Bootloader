#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#define STAGE2_POS 0x7E00
#define DAP_ADR 0x7C00
#define RM_MAX (0xffff*0x10+0xffff) //highest addressabile address in real mode
#define VGA_POS 0xB8000
#define MAX_READ_SECTORS 127

void *Memcpy(void *dst, const void *src, size_t size);

#endif
