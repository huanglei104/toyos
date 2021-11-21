#include <boot.h>
#include <string.h>

__task void task2_run(irframe_t *irf)
{
	int i = 0;
	char buf[16];

	while (1) {
		sprintf(buf, "%d", i);
		printat(buf, 0, 10);
		i++;
		sleep(1);
	}
}

__task void task3_run(irframe_t *irf)
{
	int i = 0;
	char buf[16];

	while (1) {
		sprintf(buf, "%d", i);
		printat(buf, 1, 10);
		i++;
		sleep(2);
	}
}

__task void task4_run(irframe_t *irf)
{
	int i = 0;
	char buf[16];

	while (1) {
		sprintf(buf, "%d", i);
		printat(buf, 2, 10);
		i++;
		sleep(3);
	}
}

int main()
{
	interrupt_init();
	multitask_init();

	create_task(task2_run);
	create_task(task3_run);
	create_task(task4_run);

	sti();

	while (1);
}
