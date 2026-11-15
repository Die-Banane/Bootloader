#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memcpy(void *restrict dst, const void *restrict src, size_t size);
size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);

#endif
