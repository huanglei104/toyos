#ifndef STDTYPE_H
#define STDTYPE_H

typedef __builtin_va_list va_list;
typedef char int8;
typedef unsigned char u8;
typedef short int16;
typedef unsigned short u16;
typedef int int32;
typedef unsigned int u32;
typedef long long int64;
typedef unsigned long long u64;

#define	NULL	(void*)0
#define INT_MAX	__INT_MAX__
#define INT_MIN	(-__INT_MAX__  - 1)
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

#define array_size(a) (sizeof(a) / sizeof(a[0]))

#endif
