#include <stdtype.h>

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} datetime_t;

void putc(uchar_t c);
void puts(uchar_t *s);
void putat(const uchar_t *str, uint32_t row, uint32_t col);
void printf(const char *fmt, ...);
void memset(char *str, int val, int len);
uchar_t* itoa(int i, uchar_t *buf, int radix);
uchar_t* utoa(uint32_t num, uchar_t *buf, int radix);
int atoi(char *str);
int isdigit(int c);
int isblank(int c);
int strlen(char *str);
char* strcpy(char *dst, char *src);
