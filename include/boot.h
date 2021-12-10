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
#define MEM_LIST	0x1b820

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

#define selector	u16

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
	u16 low16;
	u32 high32;
} __attribute__((packed)) byte48_t;

typedef struct {
	union {
		struct {
			u16 limit0;
			u16 base0;
			u8 base1;
			u8 type;
			u8 limit1:4;
			u8 misc:4;
			u8 base2;
		} gdt;
		struct {
			u16 offset1;
			u16 selector;
			u16 attr;
			u16 offset2;
		} idt;
	};
} __attribute__((packed)) descriptor;

typedef struct {
	u32 ip;
	u32 cs;
	u32 flags;
	u32 sp;
	u32 ss;
} irframe_t;

typedef struct {
	u32 pre;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldtsel;
	u16 t;
	u16 iomap;
} __attribute__((packed)) tss_segment;

typedef struct {
	u32 pid;
	u32 termno;
	u32 esp;
	u32 eip;
	selector ss_sel;
	selector tss_sel;
	tss_segment *tss;
} __attribute__((packed)) task_struct;

void sleep(int s);
void interrupt_init();
void multitask_init();
void create_task(int termno, void (*run)());
void inbyte(u32 port, int32 *value);
void outbyte(u32 port, int32 value);
void systime(u32 time[6]);
void do_switch(selector sel);
