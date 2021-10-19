#include <boot.h>
#include <string.h>

void kernel_entry()
{
	report_memory();

	enable_page();

	setup_idt();

	init_8259a();

	init_timer();

	init_rtc();

	enable_nmi();

	while (1);
}
