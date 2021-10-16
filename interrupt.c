#include <core.h>
#include <stdtype.h>
#include <string.h>

#define IRMAX	256
#define IRQ_DF	0x8
#define IRQ_CLK	0x20
#define IRQ_KB	0x21

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
void clk_process(struct interrupt_frame *frame)
{
	printf("%s\n", __func__);

	outbyte(0x20, 0x20);
}

static __attribute__((interrupt))
void kb_process(struct interrupt_frame *frame)
{
	int code;

	inbyte(0x60, &code);

	printf("%d\n", code);

	outbyte(0x20, 0x20);
}

void setup_idt()
{
	uint16_t idtbase[3];
	ivec_t vecs[] = {
		{IRQ_DF, df_process},
		{IRQ_CLK, clk_process},
		{IRQ_KB, kb_process},
	};

	for (int i = 0; i < IRMAX; i++) {
		idt[i].select = 0x8;
		idt[i].offset1 = low16(common_process);
		idt[i].offset2 = high16(common_process);
		idt[i].fixed8e00 = 0x8e00;
	}

	for (int i = 0; i < sizeof(vecs) / sizeof(ivec_t); i++) {
		idt[vecs[i].number].offset1 = low16(vecs[i].handler);
		idt[vecs[i].number].offset2 = high16(vecs[i].handler);
	}

	idtbase[0] = sizeof(idte_t) * IRMAX - 1;
	*(uint32_t*)(idtbase + 1) = (uint32_t)idt;

	asm volatile ("lidt %0"::"m"(idtbase));
}
