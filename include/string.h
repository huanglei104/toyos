#include <stdtype.h>

void printf(const char *fmt, ...);
void sprintf(char *buf, const char *fmt, ...);
void printat(const char *str, uint32_t row, uint32_t col);
void memset(void *addr, int val, int len);
char* itoa(int i, char *buf, int radix);
char* utoa(uint32_t num, char *buf, int radix);
int atoi(char *str);
int isdigit(int c);
int isblank(int c);
int strlen(char *str);
int strcpy(char *dst, char *src);
