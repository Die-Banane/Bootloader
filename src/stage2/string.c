#include <string.h>

void *memcpy(void *restrict dst, const void *restrict src, size_t size)
{
    unsigned char *dest = (unsigned char*)dst;
    const unsigned char *source = (const unsigned char*)src;

    for (size_t i = 0; i < size; i++)
	dest[i] = source[i];

    return dst;
}

size_t strlen(const char *str)
{
    size_t len = 0;

    for(; *str; len++, str++) continue;
    return len;
}

int strcmp(const char *s1, const char *s2)
{
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 || *p2)
    {
	if (*p1 != *p2)
	{
	    return *p1 < *p2
		? -1
		: 1;
	}

	p1++;
	p2++;
    }

    return 0;
}
