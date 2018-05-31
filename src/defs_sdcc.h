#ifndef DEFS_SDCC_H
#define DEFS_SDCC_H

#ifdef __SDCC

// SDCC doesn't have doubles, only floats :-(
#define cos cosf
#define sin sinf
#define atan atanf
#define sqrt sqrtf

// SDCC defines memset in string.h
#include <string.h>

// SDCC defines putchar, but not putch (see ex4.c)
#define putch putchar

#endif

#endif
