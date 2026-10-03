#include <stdint.h>
#include <stddef.h>
#include <memory.h>

void *Memcpy(void *dest, void *src, size_t size)
{
    for (size_t i = 0; i < size; i++)
	((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
    
    return dest;
}
