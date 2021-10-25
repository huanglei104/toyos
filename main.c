#include <boot.h>
#include <string.h>
#include <core.h>

void kernel_entry()
{
	init_gdt();

	init_stack();

	report_memory();

	enable_page();

	init_idt();

	init_task();

	init_8259a();

	init_timer();

	init_rtc();

	enable_nmi();

	while (1);
}
