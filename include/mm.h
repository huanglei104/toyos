#include <stdtype.h>

void memory_init();
void memory_dump();
void* memory_alloc(u32 size);
void memory_free(void *addr);
