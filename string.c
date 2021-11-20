#include <stdtype.h>
#include <tty.h>

#define VSTART		0xb8000
#define TEXTATTR	0xf

int strlen(const char *str)
{
	int i;

	for (i = 0; str[i]; i++);

	return i;
}

int isblank(int c)
{
	if (c == ' ' || c == '\t') return 1;

	return 0;
}

int isdigit(int c)
{
	if (c >= '0' && c <= '9') return 1;

	return 0;
}

int atoi(char *str)
{
	int data = 0, i = 0, sign = 1;

	while (isblank(*str)) str++;

	if (*str == '-' || *str == '+') sign = 1 - 2 * (*str++ == '-');

	while (isdigit(*str)) {
		if (data > INT_MAX / 10 || (data == INT_MAX / 10 && (str[i] > '7'))) {
			if (sign == -1)
				return INT_MIN;
			else
				return INT_MAX;
		}

		data = data * 10 + (*str++ - '0');
	}

	return data * sign;
}

char* itoa(int32_t i, char *buf, int radix)
{
	int tidx = 0, bidx = 0;
    char tmp[16] = {0};
    const char *map = "0123456789abcdef";

    if (i < 0 ) {
        i = -i;
        buf[bidx++] = '-';
    }

    do tmp[tidx++] = map[i % radix]; while ((i /= radix));

    for (; tidx >= 0;) buf[bidx++] = tmp[--tidx];

    return buf;
}

char* utoa(uint32_t num, char *buf, int radix)
{
	int tidx = 0, bidx = 0;
    char tmp[16] = {0};
    const char *map = "0123456789abcdef";

    do tmp[tidx++] = map[num % radix]; while ((num /= radix));

    for (; tidx >= 0;) buf[bidx++] = tmp[--tidx];

    return buf;
}

int strcpy(char *dst, char *src)
{
	int i;

	for (i = 0; src[i]; i++) dst[i] = src[i];

	return i;
}

void memset(void *addr, int val, int len)
{
	char *p = addr;

	for (int i = 0; i < len; i++) p[i] = val;
}

void vprintf(char *dest, const char *fmt, va_list va)
{
	char cval;
	uint32_t uval;
	char *strval, buf[16];
	int radix, intval;
	int offset = 0;

	for (int i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			memset(buf, 0, 16);
			if (fmt[i + 1] == 'd' || fmt[i + 1] == 'x') {
				intval = va_arg(va, int32_t);
				radix = fmt[i + 1] == 'd' ? 10 : 16;
				itoa(intval, buf, radix);
				if (dest == NULL)
					tty_puts(buf);
				else
					offset += strcpy(dest + offset, buf);

			} else if (fmt[i + 1] == 'D' || fmt[i + 1] == 'X') {
				uval = va_arg(va, uint32_t);
				radix = fmt[i + 1] == 'D' ? 10 : 16;
				utoa(uval, buf, radix);
				if (dest == NULL)
					tty_puts(buf);
				else
					offset += strcpy(dest + offset, buf);

			} else if (fmt[i + 1] == 's') {
				strval = va_arg(va, char*);
				if (dest == NULL)
					tty_puts(strval);
				else
					offset += strcpy(dest + offset, strval);

			} else if (fmt[i + 1] == 'c') {
				cval = (char)va_arg(va, uint32_t);
				if (dest == NULL)
					tty_putc(cval);
				else
					dest[offset++] = cval;
			}

			i++;

		} else {
			if (dest == NULL)
				tty_putc(fmt[i]);
			else
				dest[offset++] = fmt[i];
		}
	}
}

void sprintf(char *buf, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vprintf(buf, fmt, va);
	buf[strlen(buf)] = '\0';
	va_end(va);
}


void printf(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vprintf(NULL, fmt, va);
	va_end(va);
}

void printat(const  char *str, uint32_t row, uint32_t col)
{
	tty_putat(str, row, col);
}
