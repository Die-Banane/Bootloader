/*
 * this file is almost entirely vibe coded
 */
#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

//supports %[flags][width][.precision][length] with conversions
//d i u o x X b c s p %, flags - + space 0 #, length hh h l ll j z t
//
//Deliberately unsupported: floating point (%f %e %g %a and their upper-case
//variants), %n, and wide-character conversions (%lc, %ls). Unsupported
//conversions are printed literally rather than consuming an argument.
int printf(const char *restrict fmt, ...) __attribute__((format(printf, 1, 2)));

//same formatter as printf, takes the arguments as a va_list
int vprintf(const char *restrict fmt, va_list args);

//same formatter as printf, writes at most cap - 1 chars and a terminator,
//returns the length the untruncated output would have had
int snprintf(char *restrict dst, size_t cap, const char *restrict fmt, ...)
    __attribute__((format(printf, 3, 4)));

#endif
