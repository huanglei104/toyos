#include <stdtype.h>

#define TEXTATTR	0xf

static uint32_t position = 0xb8000;

static void __newline()
{
	position = position + 160 - (position - 0xb8000) % 160;
}

void puts(const uchar_t *str)
{
	for (int i = 0; str[i]; i++) {
		if (str[i] == '\n') {
			__newline();

		} else {
			*(uchar_t*)position = str[i];
			*(uchar_t*)(position + 1) = TEXTATTR;

			position += 2;
		}
	}
}

void putc(uchar_t c)
{
	if (c == '\n') {
		__newline();

	} else {
		*(uchar_t*)position = c;
		*(uchar_t*)(position + 1) = TEXTATTR;

		position += 2;
	}
}

uchar_t* itoa(int num, uchar_t *str, int radix)
{
	const char table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int negative = 0;
	uchar_t *start, temp;
	uchar_t *ptr = str;

	if (num == 0) {
		*ptr++ = '0';
		*ptr = '\0';
		return str;
	}

	if (num < 0){
		*ptr++ = '-';
		num *= -1;
		negative = 1;
	}

	while (num){
		*ptr++ = table[num % radix];
		num /= radix;
	}

	*ptr = '\0';
	start = negative ? str + 1 : str;
	ptr--;

	while (start < ptr){
		temp = *start;
		*start = *ptr;
		*ptr = temp;
		start++;
		ptr--;
	}

	return str;
}

uchar_t* utoa(uint32_t num, uchar_t *str, int radix)
{
	const char table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int negative = 0;
	uchar_t *start, temp;
	uchar_t *ptr = str;

	if (num == 0) {
		*ptr++ = '0';
		*ptr = '\0';
		return str;
	}

	if (num < 0){
		*ptr++ = '-';
		num *= -1;
		negative = 1;
	}

	while (num){
		*ptr++ = table[num % radix];
		num /= radix;
	}

	*ptr = '\0';
	start = negative ? str + 1 : str;
	ptr--;

	while (start < ptr){
		temp = *start;
		*start = *ptr;
		*ptr = temp;
		start++;
		ptr--;
	}

	return str;
}

void memset(uchar_t *str, int val, int len)
{
	for (int i = 0; i < len; i++) str[i] = val;
}

int printf(const uchar_t *fmt, ...)
{
	int radix;
	int cval, intval;
	uint32_t uval;
	uchar_t *strval, buf[16];
	va_list va;

	va_start(va, fmt);

	for (int i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			if (fmt[i + 1] == 'd' || fmt[i + 1] == 'x') {
				memset(buf, 0, 16);
				intval = va_arg(va, int);
				radix = fmt[i + 1] == 'd' ? 10 : 16;
				itoa(intval, buf, radix);
				puts(buf);

			} else if (fmt[i + 1] == 'u') {
				memset(buf, 0, 16);
				uval = va_arg(va, uint32_t);
				utoa(uval, buf, 10);
				puts(buf);

			} else if (fmt[i + 1] == 'p') {
				memset(buf, 0, 16);
				uval = va_arg(va, uint32_t);
				utoa(uval, buf, 16);
				puts(buf);

			} else if (fmt[i + 1] == 's') {
				strval = va_arg(va, uchar_t*);
				puts(strval);

			} else if (fmt[i + 1] == 'c') {
				cval = va_arg(va, int);
				putc(cval);
			}

			i++;

		} else {
			putc(fmt[i]);
		}
	}

	va_end(va);

	return 0;
}

char *strcpy(char *dst, char *src)
{
	for (int i = 0; src[i]; i++)
		dst[i] = src[i];

	return dst;
}

int strlen(char *str)
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

int atoi(uchar_t *str)
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
