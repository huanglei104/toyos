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
	}
}

__task void task3_run(irframe_t *irf)
{
	int i = 'A';

	while (1) {
		if (i > 'Z') i = 'A';
		printat((char*)&i, 1, 10);
		i++;
	}
}

__task void task4_run(irframe_t *irf)
{
	int i = '0';

	while (1) {
		if (i > '9') i = '0';
		printat((char*)&i, 2, 10);
		i++;
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
