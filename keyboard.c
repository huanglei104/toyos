#include <string.h>
#include <boot.h>
#include <tty.h>

typedef struct {
	int val1;
	int val2;
} kbascii_t;

typedef enum {
	key_invalid0,	key_esc,	key_num1,	key_num2,	key_bum3,
	key_num4,		key_num5,	key_num6,	key_num7,	key_num8,
	key_num9,		key_num0,	key_hyphen, key_equal,	key_packspace,
	key_tab,		key_q,		key_w,		key_e,		key_r,
	key_t,			key_y,		key_u,		key_i,		key_o,
	key_p,			key_brackets_left,		key_brackets_right,
	key_enter,		key_ctrl,	key_a,		key_s,		key_d, key_f,
	key_g,			key_h,		key_j,		key_k,		key_l,
	key_semicolon,	key_quote,	key_backquote,			key_shift_left,
	key_backslash,	key_z,		key_x, key_c,			key_v,
	key_b,			key_n,		key_m,		key_comma,	key_dot,
	key_slash,		key_shift_right,		key_invalid37,
	key_alt,		key_space,	key_capslock,			key_f1,
	key_f2,			key_f3,		key_f4,		key_f5,		key_f6,
	key_f7,			key_f8,		key_f9,		key_f10,	key_f11 = 0x57,
	key_f12 = 0x58,
} scan_code;

static kbascii_t keymap[] = {
	{0x00, 0x00}, {0x1b, 0x1b}, {0x31, 0x21}, {0x32, 0x40}, {0x33, 0x23},
	{0x34, 0x24}, {0x35, 0x25}, {0x36, 0x5e}, {0x37, 0x26}, {0x38, 0x2a},
	{0x39, 0x28}, {0x30, 0x29}, {0x2d, 0x5f}, {0x3d, 0x2b}, {0x08, 0x08},
	{0x09, 0x09}, {0x71, 0x51}, {0x77, 0x57}, {0x65, 0x45}, {0x72, 0x52},
	{0x74, 0x54}, {0x79, 0x59}, {0x75, 0x55}, {0x69, 0x49}, {0x6f, 0x4f},
	{0x70, 0x50}, {0x5b, 0x7b}, {0x5d, 0x7d}, {0x0a, 0x0a}, {0x00, 0x00},
	{0x61, 0x41}, {0x73, 0x53}, {0x64, 0x44}, {0x66, 0x46}, {0x67, 0x47},
	{0x68, 0x48}, {0x6a, 0x4a}, {0x6b, 0x48}, {0x6c, 0x4c}, {0x3b, 0x3a},
	{0x27, 0x22}, {0x60, 0x7e}, {0x00, 0x00}, {0x5c, 0x7c}, {0x7a, 0xaa},
	{0x78, 0x58}, {0x63, 0x43}, {0x76, 0x56}, {0x62, 0x42}, {0x6e, 0x4e},
	{0x6d, 0x4d}, {0x2c, 0x3c}, {0x2e, 0x3e}, {0x2f, 0x3f}, {0x00, 0x00},
	{0x00, 0x00}, {0x00, 0x00}, {0x20, 0x20}, {0x00, 0x00}, {0x00, 0x00},
};

static int ctrl_status = 0;
static int shift_status = 0;
static int alt_status = 0;
static int capslock_status = 0;

void keyboard_process()
{
	int32_t broken = 0, code = 0;
	uint32_t ascii = 0;

	inbyte(0x60, &code);

	/*左右ctrl/alt做相同处理*/
	if (code == 0xe0)
		return;

	broken = code & 0x7f;

	if (code & 0x80) {
		if (broken == key_ctrl)
			ctrl_status = 0;
		else if (broken == key_alt)
			alt_status = 0;
		else if (broken == key_shift_left || broken == key_shift_right)
			shift_status = 0;

	} else {
		if (code == key_capslock)
			capslock_status = !capslock_status;
		else if (code == key_ctrl)
			ctrl_status = 1;
		else if (code == key_alt)
			alt_status = 1;
		else if (code == key_shift_left || code == key_shift_right)
			shift_status = 1;

		else {
			if (code >= key_f1 && code <= key_f4 && alt_status) {
				tty_switch(code - key_f1);
				return;
			}

			if (shift_status || capslock_status)
				ascii = keymap[code].val2;
			else
				ascii = keymap[code].val1;

			char buf[4] = {0};
			sprintf(buf, "%c", ascii);
			tty_put_at_screen(buf);
		}
	}
}
