#include <stdtype.h>
#include <tty.h>
#include <task.h>

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

void memcpy(char *dst, char *src, int len)
{
	for (int i = 0; i < len; i++)
		dst[i] = src[i];
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

char* itoa(int32 i, char *buf, int radix)
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

char* utoa(u32 num, char *buf, int radix)
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

int vsprintf(char *restrict dst, const char *restrict fmt, va_list ap)
{
	char *strval, buf[16];
	int radix, intval;
	u32 uval;

	for (int i = 0, offset = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			memset(buf, 0, 16);
			if (fmt[i + 1] == 'd' || fmt[i + 1] == 'x') {
				intval = va_arg(ap, int32);
				radix = fmt[i + 1] == 'd' ? 10 : 16;
				itoa(intval, buf, radix);
				offset += strcpy(dst+ offset, buf);

			} else if (fmt[i + 1] == 'D' || fmt[i + 1] == 'X') {
				uval = va_arg(ap, u32);
				radix = fmt[i + 1] == 'D' ? 10 : 16;
				utoa(uval, buf, radix);
				offset += strcpy(dst + offset, buf);

			} else if (fmt[i + 1] == 's') {
				strval = va_arg(ap, char*);
				offset += strcpy(dst + offset, strval);

			} else if (fmt[i + 1] == 'c') {
				intval = va_arg(ap, int32);
				dst[offset++] = intval;
			}

			i++;

		} else {
			dst[offset++] = fmt[i];
		}
	}

	return 0;
}

void sprintf(char *buf, const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vsprintf(buf, fmt, va);
	buf[strlen(buf)] = '\0';
	va_end(va);
}


void printf(const char *fmt, ...)
{
	va_list ap;
	char buf[128] = {0};

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	tty_put_at_terminal(get_tty(), buf);
}

void printat(const char *str, int row, int col)
{
	tty_put_with_xy(get_tty(), str, row, col);
}
