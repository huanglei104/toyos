#include <boot.h>
#include <string.h>

typedef struct {
	u32 addr;
	u32 addr_high;
	u32 len;
	u32 len_high;
	u32 type;
} __attribute((packed)) e820entry;

typedef u32 size_t;
typedef struct mnode mnode;

typedef struct mnode {
	size_t *s;
	mnode *prev;
	mnode *next;
} mnode;

/*内核安排在3M以下的空间，堆从3M开始*/
static u32 free = 0x300000;

static mnode link;

void memory_init()
{
	int max = 0;
	int nr_entry = *(int*)0x7b00;
	e820entry *e = (void*)0x7b04;

	for (int i = 0; i < nr_entry; i++) {
		if (e[i].len  > e[max].len)
			max = i;
	}

	if (e[max].addr > free)
		free = e[max].addr;

	memset((void*)MEM_LIST, 0, 0x20000 - MEM_LIST);
}

void* memory_alloc(u32 size)
{
	size_t *s;
	mnode *n = link.next;

	while (n) {
		if (*n->s >= size) {
			n->prev->next = n->next;
			if (n->next)
				n->next->prev = n->prev;

			return n->s + 1;
		}

		n = n->next;
	}

	s = (size_t*)free;
	*s = size;
	free += size + 4; //4 = sizeof(size_t)

	return s + 1;
}

static void mnode_next_merge(mnode *n)
{
	if (n->next == NULL)
		return;

	if ((u32)n->s + 4 + *n->s == (u32)n->next->s) {
		*n->s += *n->next->s + 4;
		n->next->s = NULL;
		n->next = n->next->next;
		n->next->next->prev = n;
	}
}

static void link_tail_free()
{
	mnode *n = link.next;

	while (n->next) n = n->next;

	if ((u32)n->s + *n->s + 4 == free) {
		free -= *n->s + 4;
		n->s = NULL;
		n->prev->next = NULL;
	}
}

static mnode* mnode_get()
{
	mnode *n = (mnode*)MEM_LIST;

	while (n->s) n++;

	n->prev = NULL;
	n->next = NULL;

	return n;
}

static void link_sort_insert(mnode *n)
{
	mnode *cur = link.next;

	while (cur->s < n->s && cur->next)
		cur = cur->next;

	if (cur->s >= n->s) {
		n->next = cur;
		n->prev = cur->prev;
		cur->prev = n;
		n->prev->next = n;

	} else {
		cur->next = n;
		n->prev = cur;
	}

	mnode_next_merge(n);
	mnode_next_merge(n->prev);
}

void memory_free(void *addr)
{
	mnode	*n;
	size_t	*s;

	s = addr - 4;
	n = mnode_get();
	n->s = s;

	if (link.next == NULL) {
		link.next = n;
		n->prev = &link;

	} else
		link_sort_insert(n);

	link_tail_free();
}

void memory_dump()
{
	mnode *n = link.next;

	while (n) {
		printf("mnode size %X %d\n", n->s, *n->s);
		n = n->next;
	}

	printf("free %X\n", free);
}
