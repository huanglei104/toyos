#include <boot.h>
#include <string.h>

#define INTERRUPT_MAX	0x3c
#define	VECTOR_TIMER	0x20
#define VECTOR_KEYBOARD	0x21
#define VECTOR_CLOCK	0x28

extern void schedule();
extern void keyboard_process();

volatile int jiffies;

static __interrupt
void interrupt_common_handler(irframe_t *irf)
{
	printf("%s\n", __func__);
}

static __interrupt
void interrupt_keyboard_handler(irframe_t *irf)
{
	keyboard_process();
	outbyte(0x20, 0x20);
}

static __interrupt
void interrupt_clock_handler(irframe_t *irf)
{
	char buf[32];
	uint32_t time[6];

	read_time(time);

	sprintf(buf, "%d/%d/%d-%d:%d:%d",
			time[0], time[1], time[2],
			time[3], time[4], time[5]);

	printat(buf, 10, 40);
}

static __interrupt
void interrupt_timer_handler(irframe_t *irf)
{
	jiffies++;

	outbyte(0x20, 0x20);
	schedule();
}

static void* handler_list[INTERRUPT_MAX] = {
	[VECTOR_TIMER] = interrupt_timer_handler,
	[VECTOR_KEYBOARD] = interrupt_keyboard_handler,
	[VECTOR_CLOCK] = interrupt_clock_handler,
};

void interrupt_init()
{
	descriptor *table = (void*)IDT_BASE;
	byte48_t desc_ptr = {INTERRUPT_MAX * 8 - 1, IDT_BASE};

	for (int i = 0; i < INTERRUPT_MAX; i++) {
		table[i].idt.selector = 0x8;
		table[i].idt.attr = 0x8e00;
		if (handler_list[i]) {
			table[i].idt.offset1 = (uint32_t)handler_list[i];
			table[i].idt.offset2 = (uint32_t)handler_list[i] >> 16;

		} else {
			table[i].idt.offset1 = (uint32_t)interrupt_common_handler;
			table[i].idt.offset2 = (uint32_t)interrupt_common_handler >> 16;
		}
	}

	asm volatile("lidt %0"::"m"(desc_ptr));
}
