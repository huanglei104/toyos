#include <boot.h>
#include <string.h>

void kernel_entry()
{
	report_memory();

	enable_page();

	while (1);
}
