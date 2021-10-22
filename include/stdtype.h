#ifndef STDTYPE_H
#define STDTYPE_H

typedef __builtin_va_list va_list;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned char uchar_t;

#define	NULL	(void*)0
#define INT_MAX	__INT_MAX__
#define INT_MIN	(-__INT_MAX__  - 1)
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

#define low16(addr) ((uint16_t)((uint32_t)(addr)))
#define high16(addr) ((uint16_t)(((uint32_t)(addr)) >> 16))

#define array_size(a) (sizeof(a) / sizeof(a[0]))

#endif
