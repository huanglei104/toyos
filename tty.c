#include <boot.h>
#include <string.h>
#include <task.h>

#define NR_TTY	4
#define TTY		tty[termno]
#define SCREEN	tty[current]

typedef struct {
	u32 start;
	u32 end;
	u32 width;
	u32 height;
	u32 display_y;
	u32 x;
	u32 y;
	u8 color;
} struct_tty;

static int current;
static struct_tty tty[NR_TTY];

void tty_update_cursor()
{
	int pos;

	pos = (SCREEN.start - 0xb8000) / 2 +  SCREEN.y * 80 + SCREEN.x;

	outbyte(0x3d4, 0xe);
	outbyte(0x3d5, pos >> 8);
	outbyte(0x3d4, 0xf);
	outbyte(0x3d5, pos);

	((char*)(SCREEN.start + SCREEN.y * 160 + SCREEN.x * 2))[1] = 0x8c;
}

/*相对于0xb8000的字符的偏移*/
void tty_set_screen(int offset)
{
	outbyte(0x3d4, 0xc);
	outbyte(0x3d5, offset >> 8);
	outbyte(0x3d4, 0xd);
	outbyte(0x3d5, offset);
}

void tty_init()
{
	for (int i = 0; i < NR_TTY; i++) {
		tty[i].width = 80;
		tty[i].height = 50;
		tty[i].color = 0xf;
		tty[i].display_y = 0;
		tty[i].x = 0;
		tty[i].y = 0;
		tty[i].start = 0xb8000 + i * 8000;
		tty[i].end = tty[i].start + 7999;
	}

	memset((void*)0xb8000, 0, 0x8000);

	current = 0;

	tty_update_cursor();
}

void tty_switch(int termno)
{
	int offset;

	if (termno < 0 || termno > NR_TTY - 1 || termno == current)
		return;

	current = termno;
	offset = (TTY.start - 0xb8000) / 2;

	tty_set_screen(offset);

	tty_update_cursor();
}

void tty_scroll(int termno, int dir)
{
	int offset =
		(TTY.start - 0xb8000) / 2 + (TTY.display_y) * 80;

	tty_set_screen(offset);
}

void tty_put_at_terminal(int termno, const char *str)
{
	char *addr;

	for (int i = 0; str[i]; i++) {
		if (str[i] == '\n') {
			TTY.x = 0;
			TTY.y++;
			continue;
		}

		if (TTY.x >= TTY.width) {
			TTY.x = 0;
			TTY.y++;
		}

		if (TTY.y >= TTY.height)
			SCREEN.y = 0;

		addr = (void*)(TTY.start + TTY.y * 160 + TTY.x * 2);
		addr[0] = str[i];
		addr[1] = TTY.color;
		TTY.x++;
	}

	if (TTY.y >= 25) {
		TTY.display_y = TTY.y - 24;
		tty_scroll(termno, 1);
	}
}

void tty_put_with_xy(int termno, const char *str, int x, int y)
{
	int screen_x, screen_y;

	if (x >= SCREEN.width)
		x = SCREEN.width - 1;
	if (y >= SCREEN.height)
		y = SCREEN.height - 1;

	screen_y = SCREEN.y;
	screen_x = SCREEN.x;
	SCREEN.y = y;
	SCREEN.x = x;

	tty_put_at_terminal(termno, str);

	SCREEN.y = screen_y;
	SCREEN.x = screen_x;
}

void tty_put_at_screen(char *str)
{
	tty_put_at_terminal(current, str);

	tty_update_cursor();
}
