#include <string.h>

int a  = 10;

typedef struct {
	int p:1;
	int rw:1;
	int us:1;
	int pwt:1;
	int pcd:1;
	int a:1;
	int d:1;
	int pat:1;
	int g:1;
	int avl:3;
	int base:20;
} pde_t;

typedef pde_t pte_t;

pde_t pdt[1024] __attribute__ ((aligned (0x1000)));

const static int *ardsn = (int*)0x7b00;

typedef struct {
	unsigned int addr1;
	unsigned int addr2;
	unsigned int len1;
	unsigned int len2;
	int type;
} ards_t;

void report_mm()
{
	ards_t *ards = (ards_t*)0x7b04;

	for (int i = 0; i < *ardsn; i++) {
		printf("addr:%p %x %u\n", ards[i].addr1, ards[i].len1, ards[i].type);
	}

	printf("pdt %x\n", &pdt);
}

void enable_page()
{
}
