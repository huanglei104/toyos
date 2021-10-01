#include <string.h>

void report_mm();

void kernel_entry()
{

	printf("entry %x\n", kernel_entry);
	report_mm();

	printf("hello^world!\n");

	while (1);
}
