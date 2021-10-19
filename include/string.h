#include <stdtype.h>

void putc(uchar_t c);
void puts(uchar_t *s);
void putat(const uchar_t *str, uint32_t row, uint32_t col);
void printf(const char *fmt, ...);
void memset(char *str, int val, int len);
char* itoa(int num, uchar_t *str, int radix);
char* utoa(uint32_t num, char *str, int radix);
int atoi(char *str);
int isdigit(int c);
int isblank(int c);
int strlen(char *str);
char* strcpy(char *dst, char *src);
