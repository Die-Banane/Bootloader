#include <stdint.h>
#include <memory.h>

void *Memcpy(void *dst, const void *src, size_t size)
{
    uint8_t *dest = (uint8_t*)dst;
    const uint8_t *source = (uint8_t*)src;
    
    for (size_t i = 0; i < size; i++)
	dest[i] = source[i];

    return dst;
}
