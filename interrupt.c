#include <core.h>
#include <string.h>

#define IRMAX		256
#define IRQ_DF		0x8
#define IRQ_CLK		0x20
#define IRQ_KB		0x21
#define IRQ_RTC		0x28
#define IRQ_MOUSE	0x2c

#define KBV_CTRL		0x1d
#define KBV_SHIFT		0x2a
#define KBV_CAPSLOCK	0x3a

struct interrupt_frame;

typedef struct {
	int port;
	int value;
} iokv_t;

typedef struct {
	int number;
	void *handler;
} ivec_t;

typedef struct {
	uint16_t offset1;
	uint16_t select;
	uint16_t fixed8e00;
	uint16_t offset2;
} idte_t;

typedef struct {
	int val1;
	int val2;
} kbascii_t;

static idte_t idt[IRMAX] __attribute__((aligned(8)));

static __attribute__((interrupt))
void common_process(struct interrupt_frame *frame)
{
	printf("%s\n", __func__);
}

static __attribute__((interrupt))
void df_process(struct interrupt_frame *frame)
{
	printf("%s\n", __func__);
}

static __attribute__((interrupt))
void timer_process(struct interrupt_frame *frame)
{
	uchar_t buf[8] = {0};
	static int i = 0;

	itoa(i, buf, 10);
	putat(buf, 24, 40);
	i++;

	outbyte(0x20, 0x20);
}

static __attribute__((interrupt))
void kbd_process(struct interrupt_frame *frame)
{
	uchar_t ascii;
	uint32_t code;
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

	inbyte(0x60, &code);

	if (code & 0x80) {
		// breakcode
		//
	} else if (code == KBV_CAPSLOCK) {
		keymap[KBV_CAPSLOCK].val1 = !keymap[KBV_CAPSLOCK].val1;

	} else {
		if (keymap[KBV_CAPSLOCK].val1)
			ascii = keymap[code].val2;
		else
			ascii = keymap[code].val1;

		putc(ascii);
	}

	outbyte(0x20, 0x20);
}

static __attribute__((interrupt))
void rtc_process(struct interrupt_frame *frame)
{
	printf("%s\n", __func__);

	asm volatile("in al, 0x71");

	outbyte(0x20, 0x20);
	outbyte(0xa0, 0x20);
}

static __attribute__((interrupt))
void mouse_process(struct interrupt_frame *frame)
{
	printf("%s\n", __func__);

	outbyte(0x20, 0x20);
	outbyte(0xa0, 0x20);
}

void setup_idt()
{
	uint16_t idtbase[3];
	ivec_t vecs[] = {
		{IRQ_DF, df_process},
		{IRQ_CLK, timer_process},
		{IRQ_KB, kbd_process},
		{IRQ_RTC, rtc_process},
		{IRQ_MOUSE, mouse_process},
	};

	for (int i = 0; i < IRMAX; i++) {
		idt[i].select = 0x8;
		idt[i].offset1 = low16(common_process);
		idt[i].offset2 = high16(common_process);
		idt[i].fixed8e00 = 0x8e00;
	}

	for (int i = 0; i < array_size(vecs); i++) {
		idt[vecs[i].number].offset1 = low16(vecs[i].handler);
		idt[vecs[i].number].offset2 = high16(vecs[i].handler);
	}

	idtbase[0] = sizeof(idte_t) * IRMAX - 1;
	*(uint32_t*)(idtbase + 1) = (uint32_t)idt;

	asm volatile ("lidt %0"::"m"(idtbase));
}
