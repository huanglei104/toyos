#include <boot.h>
#include <stdtype.h>

#define	TTY_WIDTH		80
#define TTY_HEIGHT		25
#define TTY_MEM_START	(char*)0xb8000
#define TTY_MEM_END		(char*)0xbffff

static char color = 0xf;
static char *position = TTY_MEM_START;

static void tty_newline()
{
	position = position + 160 - (position - TTY_MEM_START) % 160;
}

void tty_puts(const char *str)
{
	for (int i = 0; str[i]; i++) {
		if (str[i] == '\n')
			tty_newline();

		else {
			position[0] = str[i];
			position[1] = color;

			position += 2;
			if (position > TTY_MEM_END) position = TTY_MEM_START;
		}
	}
}

void tty_putc(const char c)
{
	char buf[2] = {c, 0};

	tty_puts(buf);
}

void tty_putat(const char *str, uint32_t row, uint32_t col)
{
	char *pos;

	if (row > TTY_HEIGHT - 1) row = 0;
	if (col > TTY_WIDTH - 1) col = 0;

	pos = (char*)(row * 160 + col * 2 + TTY_MEM_START);

	for (int i = 0; str[i]; i++) {
		if (str[i] == '\n') {
			row++;
			col = 0;
			pos = (char*)(row * 160 + col * 2 + TTY_MEM_START);

		} else {
			pos[0] = str[i];
			pos[1] = color;
		}

		pos += 2;
	}
}
