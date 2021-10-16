#include <boot.h>
#include <string.h>

void kernel_entry()
{
	report_memory();

	enable_page();

	setup_idt();

	init_8259a();

	while (1);
}
