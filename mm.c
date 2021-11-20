#include <stdtype.h>

static uint32_t free_mem_start = 0x300000;

void* kalloc(uint32_t size)
{
	void *p = (void*)free_mem_start;

	free_mem_start += size;

	return p;
}
