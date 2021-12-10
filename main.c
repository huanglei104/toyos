#include <boot.h>
#include <tty.h>
#include <string.h>
#include <mm.h>

__task void task2_run(irframe_t *irf)
{
	int i = 0;

	while (1) {
		printf("%d\n", i++);
		sleep(1);
	}
}

__task void task3_run(irframe_t *irf)
{
	int i = 0;

	while (1) {
		printf("%d\n", i++);
		sleep(2);
	}
}

__task void task4_run(irframe_t *irf)
{
	int i = 0;

	while (1) {
		printf("%d\n", i++);
		sleep(3);
	}
}

int main()
{
	tty_init();
	memory_init();
	interrupt_init();
	multitask_init();

	create_task(0, task2_run);
	create_task(1, task3_run);
	create_task(2, task4_run);

	sti();

	while (1);
}
