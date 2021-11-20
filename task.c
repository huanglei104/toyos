#include <mm.h>
#include <boot.h>
#include <string.h>

#define TASK_MAX	20

int total = 0;
int current = 0;

tss_segment *tss = (void*)TSS_BASE;
task_struct *task_list = (void*)TASK_BASE;

selector gdt_add_entry(uint32_t base, uint32_t limit, uint16_t attr)
{
	descriptor *e;
	selector sel = 0;
	byte48_t desc_ptr = {0};

	asm volatile("sgdt %0;":"=m"(desc_ptr));

	e = (void*)(desc_ptr.high32 + desc_ptr.low16 + 1);
	e->gdt.limit0 = limit;
	e->gdt.limit1 = limit >> 16;
	e->gdt.base0 = base;
	e->gdt.base1 = base >> 16;
	e->gdt.base2 = base >> 24;
	e->gdt.type = attr;
	e->gdt.misc = attr >> 8;

	desc_ptr.low16 += 8;

	asm volatile("lgdt %0"::"m"(desc_ptr));

	sel = ((desc_ptr.low16 - 7) / 8) << 3;

	return sel;
}

void multitask_init()
{
	memset(tss, 0, TSS_LIMIT - TSS_BASE);
	memset(task_list, 0, TASK_LIMIT - TASK_BASE);

	// tss number is 20
	for (int i = 0; i < TASK_MAX; i++) {
		tss[i].cs = KERNEL_CODE_SEL;
		tss[i].ds = KERNEL_DATA_SEL;
		tss[i].es = KERNEL_DATA_SEL;
		tss[i].fs = KERNEL_DATA_SEL;
		tss[i].gs = KERNEL_DATA_SEL;
		tss[i].cr3 = PDT_BASE;
	}

	task_list[0].tss_sel =
		gdt_add_entry(TSS_BASE, sizeof(tss_segment), GDT_TYPE_TSS);
	asm volatile("ltr ax;"::"a"(task_list[0].tss_sel));

	total++;
}

void create_task(void (*run)())
{
	uint32_t addr = (uint32_t)kalloc(STACK_MAX);
	uint32_t limit = ((addr - 0x1000) / 0x1000) & 0xfffff;

	tss[total].ss = gdt_add_entry(0, limit, GDT_TYPE_STACK);
	tss[total].esp = addr + STACK_MAX - 1;
	tss[total].eip = (uint32_t)run;
	tss[total].eflags = 0x200;

	task_list[total].ss_sel = tss[total].ss;
	task_list[total].tss_sel = gdt_add_entry(
			(uint32_t)&tss[total],
			sizeof(tss_segment),
			GDT_TYPE_TSS);

	task_list[total].esp = tss[total].esp;
	task_list[total].eip = tss[total].eip;
	task_list[total].tss = &tss[total];

	total++;
}

void schedule()
{
	if (total == 1) return;

	if (++current == total)
		current = 1;

	do_switch(task_list[current].tss_sel);
}
