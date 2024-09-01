#ifndef LIBGO_CORE_CTYPE_H_
#define LIBGO_CORE_CTYPE_H_

#define bool int
#define true 1
#define false 0

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef unsigned long uintptr;

#define LEN(p) (sizeof(p) / sizeof(p[0]))

#endif  // LIBGO_CORE_CTYPE_H_
