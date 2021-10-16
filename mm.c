#include <string.h>

#define	PDE_NUM	1024
#define PTE_NUM	1024
#define PT_NUM	1024

typedef struct {
	unsigned int p:1;
	unsigned int rw:1;
	unsigned int us:1;
	unsigned int pwt:1;
	unsigned int pcd:1;
	unsigned int a:1;
	unsigned int d:1;
	unsigned int pat:1;
	unsigned int g:1;
	unsigned int avl:3;
	unsigned int base:20;
} pde_t;

typedef pde_t pte_t;

typedef struct {
	unsigned int addr1;
	unsigned int addr2;
	unsigned int len1;
	unsigned int len2;
	int type;
} ards_t;

pde_t pd[PDE_NUM] __attribute__ ((aligned(0x1000)));
pte_t pt[PT_NUM][PTE_NUM] __attribute__ ((aligned(0x1000)));

const static unsigned int *ardsn = (unsigned int*)0x7b00;

void report_memory()
{
	int k, m;
	ards_t *ards = (ards_t*)0x7b04;

	for (int i = 0; i < *ardsn; i++) {
		if (ards[i].type == 1) {
			k = ards[i].len1 / 0x1000;
			m = ards[i].len1 / 0x100000;
			printf("address:0x%p length:%d.%dM\n", ards[i].addr1, m, k);
		}
	}
}

void enable_page()
{
	unsigned int pdaddr = (unsigned int)&pd;
	unsigned int pbase = 0x0;

	for (int i = 0; i < PDE_NUM; i++) {
		pd[i].p = 1;
		pd[i].rw = 1;
		pd[i].us = 1;
		pd[i].base = (unsigned int)&pt[i] >> 12;

		for (int j = 0; j < PTE_NUM; j++) {
			pt[i][j].p = 1;
			pt[i][j].rw = 1;
			pt[i][j].us = 1;
			pt[i][j].base = pbase >> 12;

			pbase += 0x1000;
		}
	}

	asm __volatile__("mov eax, %0"::"m"(pdaddr));
	asm __volatile__(
			"mov cr3, eax;"
			"mov eax, cr0;"
			"or eax, 0x80000000;"
			"mov cr0, eax;"
			);
}
