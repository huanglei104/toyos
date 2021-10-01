typedef __builtin_va_list va_list;

#define	NULL	(void*)0
#define INT_MAX	__INT_MAX__
#define INT_MIN	(-__INT_MAX__  - 1)
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)
