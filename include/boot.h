#define enable_nmi() asm volatile("sti");
void report_memory();
void enable_page();
void setup_idt();
void init_8259a();
void init_timer();
void init_rtc();
