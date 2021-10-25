#define enable_nmi() asm volatile("sti");

void init_gdt();
void init_idt();
void report_memory();
void enable_page();
void init_stack();
void init_8259a();
void init_timer();
void init_rtc();
void init_task();
