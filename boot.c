#include <stdtype.h>
#include <string.h>

#define MEM_MAX		0x1000000
#define STACK_TOP	0x9ffff
#define STACK_SIZE	0x19000

#define CODE_SEL	0x8
#define DATA_SEL	0x10
#define STACK0_SEL	0x18
#define STACK1_SEL	0x20
#define TASK0_SEL	0x28
#define TASK1_SEL	0x30

#define GDTE_MAX		10
#define GDT_TYPE_CODE	0x98
#define GDT_TYPE_DATA	0x92
#define GDT_TYPE_STACK	0x96
#define GDT_TYPE_TSS	0x89

#define PTE_MAX		0x400
#define	PDE_MAX		(MEM_MAX / 0x1000 / PTE_MAX)

typedef struct {
	uint16_t limit0;
	uint16_t base0;
	uint8_t base1;
	uint8_t type;
	uint8_t limit1:4;
	uint8_t misc:4;
	uint8_t base2;
} __attribute__((packed)) gdte_t;

typedef struct {
	uint16_t pre;
	uint16_t reserve1;
	uint32_t esp0;
	uint16_t ss0;
	uint16_t reserve2;
	uint32_t esp1;
	uint16_t ss1;
	uint16_t reserve3;
	uint32_t esp2;
	uint16_t ss2;
	uint16_t reserve4;
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
	uint16_t es;
	uint16_t reservees;
	uint16_t cs;
	uint16_t reservecs;
	uint16_t ss;
	uint16_t reservess;
	uint16_t ds;
	uint16_t reserveds;
	uint16_t fs;
	uint16_t reservefs;
	uint16_t gs;
	uint16_t reservegs;
	uint16_t ldtsel;
	uint16_t reserve6;
	uint16_t reserve:15;
	uint16_t t:1;
	uint16_t iomap;
} __attribute__((packed)) tss_t;

typedef struct {
	unsigned int p:1;
	unsigned int rw:1;
	unsigned int us:1;
	unsigned int pwt:1;
	unsigned int pcd:1;
	unsigned int a:1;
	unsigned int d:1;
	unsigned int pat:1;
	unsigned int g:1;
	unsigned int avl:3;
	unsigned int base:20;
} pde_t;

typedef pde_t pte_t;

typedef struct {
	uint32_t addr1;
	uint32_t addr2;
	uint32_t len1;
	uint32_t len2;
	uint32_t type;
} ards_t;

static gdte_t gdt[GDTE_MAX];
static tss_t tasks[2] = {0};
static uint32_t *ardsn = (uint32_t*)0x7b00;

void init_gdt()
{
	gdt[1].limit0 = 0xffff;
	gdt[1].limit1 = 0xf;
	gdt[1].type = GDT_TYPE_CODE;
	gdt[1].misc = 0xc;

	gdt[2].limit0 = 0xffff;
	gdt[2].limit1 = 0xf;
	gdt[2].type = GDT_TYPE_DATA;
	gdt[2].misc = 0xc;

	gdt[3].limit0 = (STACK_TOP - STACK_SIZE - 0xfff) / 0x1000;
	gdt[3].type = GDT_TYPE_STACK;
	gdt[3].misc = 0xc;

	gdt[4].type = GDT_TYPE_STACK;
	gdt[4].misc = 0x4;

	gdt[5].base0 = low16(&tasks[0]);
	gdt[5].base1 = (uint8_t)((uint32_t)&tasks[0] >> 16);
	gdt[5].base2 = (uint8_t)((uint32_t)&tasks[0] >> 24);
	gdt[5].limit0 = sizeof(tss_t) - 1;
	gdt[5].type = GDT_TYPE_TSS;

	gdt[6].base0 = low16(&tasks[1]);
	gdt[6].base1 = (uint8_t)((uint32_t)&tasks[1] >> 16);
	gdt[6].base2 = (uint8_t)((uint32_t)&tasks[1] >> 24);
	gdt[6].limit0 = sizeof(tss_t) - 1;
	gdt[6].type = GDT_TYPE_TSS;

	gdt[0].limit0 = sizeof(gdte_t) * 7 - 1;
	*(uint32_t*)(&gdt[0].base0) = (uint32_t)gdt;

	//jmp sel:offset 指令的长度为7字节
	asm volatile(
			"lgdt %0;"
			"jmp %1:$ + 7;"
			"mov ax, %2;"
			"mov ds, ax;"
			"mov es, ax;"
			"mov fs, ax;"
			"mov gs, ax;"::"m"(gdt),"i"(CODE_SEL),"i"(DATA_SEL)
			);
}

void init_stack()
{
	asm volatile(
			"pop eax;"
			"mov bx, %0;"
			"mov ss, bx;"
			"mov esp, %1;"
			"push eax;"::"i"(STACK0_SEL),"i"(STACK_TOP)
			);
}

struct interrupt_frame {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

static __attribute__((interrupt))
void task1_run(struct interrupt_frame *f)
{
	printf("pre %d\n", tasks[1].pre);
	printf("%s\n", __func__);
}

void init_task()
{
	tasks[1].cs = CODE_SEL;
	tasks[1].eip = (uint32_t)task1_run;
	tasks[1].ds = DATA_SEL;
	tasks[1].es = DATA_SEL;
	tasks[1].fs = DATA_SEL;
	tasks[1].gs = DATA_SEL;
	tasks[1].ss = STACK1_SEL;
	tasks[1].esp = 0x4ff;

	asm volatile(
			"mov eax, cr3;":"=a"(tasks[1].cr3)
			);

	asm volatile(
			"mov ax, %0;"
			"ltr ax;"::"i"(TASK0_SEL)
			);
}

void report_memory()
{
	int k, m;
	ards_t *ards = (ards_t*)0x7b04;

	for (int i = 0; i < *ardsn; i++) {
		if (ards[i].type == 1) {
			k = ards[i].len1 / 0x1000;
			m = ards[i].len1 / 0x100000;
			printf("address:0x%p length:%d.%dM\n", ards[i].addr1, m, k);
		}
	}
}

void enable_page()
{
	uint32_t pbase = 0x0;
	uint32_t consume = ((MEM_MAX / 0x1000) + 1) * 4;
	uint32_t gdtbase = STACK_TOP - STACK_SIZE - consume;

	gdtbase -= (gdtbase % 0x1000);
	memset((void*)gdtbase, 0, 0x1000);

	pde_t *pde = (pde_t*)gdtbase;
	pte_t *pte = (pte_t*)(gdtbase + 0x1000);

	for (int i = 0; i < PDE_MAX; i++) {
		pde[i].p = 1;
		pde[i].rw = 1;
		pde[i].us = 1;
		pde[i].base = (uint32_t)&pte[i] >> 12;

		for (int j = 0; j < PTE_MAX; j++) {
			pte[j].p = 1;
			pte[j].rw = 1;
			pte[j].us = 1;
			pte[j].base = pbase >> 12;

			pbase += 0x1000;
		}

		pte += 0x400;
	}

	asm __volatile__(
			"mov eax, %0;"
			"mov cr3, eax;"
			"mov eax, cr0;"
			"or eax, 0x80000000;"
			"mov cr0, eax;"::"m"(gdtbase)
			);
}
