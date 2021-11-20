#include <stdtype.h>

#define __task		__attribute__((interrupt))
#define __interrupt	__attribute__((interrupt))

#define GDT_BASE	0
#define GDT_LIMIT	0x4ff
#define STACK_LIMIT	0x500
#define STACK_TOP	0x104ff
#define IDT_BASE	0x10500
#define IDT_LIMIT	0x1068f
#define TASK_BASE	0x10690
#define TASK_LIMIT	0x10fff
#define PDT_BASE	0x11000
#define PDT_LIMIT	0x1a8ff
#define TSS_BASE	0x1b000
#define TSS_LIMIT	0x1b81f

#define KERNEL_CODE_SEL		0x8
#define KERNEL_DATA_SEL		0x10
#define KERNEL_STACK_SEL	0x18

#define MEM_MAX		0x1000000
#define STACK_MAX	0x10000
#define TSS_MAX		0x14

#define GDT_TYPE_CODE		0xc98
#define GDT_TYPE_DATA		0xc92
#define GDT_TYPE_STACK		0xc96
#define GDT_TYPE_TSS		0x89

#define selector	uint16_t

__attribute__((always_inline))
inline void sti()
{
	asm volatile("sti");
}

__attribute__((always_inline))
inline void cli()
{
	asm volatile("cli");
}

__attribute__((always_inline))
inline void hlt()
{
	asm volatile("hlt");
}

typedef struct {
	uint16_t low16;
	uint32_t high32;
} __attribute__((packed)) byte48_t;

typedef struct {
	union {
		struct {
			uint16_t limit0;
			uint16_t base0;
			uint8_t base1;
			uint8_t type;
			uint8_t limit1:4;
			uint8_t misc:4;
			uint8_t base2;
		} gdt;
		struct {
			uint16_t offset1;
			uint16_t selector;
			uint16_t attr;
			uint16_t offset2;
		} idt;
	};
} __attribute__((packed)) descriptor;

typedef struct {
	uint32_t ip;
	uint32_t cs;
	uint32_t flags;
	uint32_t sp;
	uint32_t ss;
} irframe_t;

typedef struct {
	uint32_t pre;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldtsel;
	uint16_t t;
	uint16_t iomap;
} __attribute__((packed)) tss_segment;

typedef struct {
	uint32_t esp;
	uint32_t eip;
	selector ss_sel;
	selector tss_sel;
	tss_segment *tss;
} __attribute__((packed)) task_struct;

void interrupt_init();
void multitask_init();
void create_task(void (*run)());
void inbyte(uint32_t port, uint8_t *value);
void outbyte(uint32_t port, uint16_t value);
void read_time(uint8_t time[6]);
void do_switch(selector sel);
