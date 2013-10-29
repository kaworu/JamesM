#include <paging.h>
#include <heap.h>

/* A bitset of frames - used or free. */
uint32_t	*frames;
uint32_t	nframes;

/* Defined in kmalloc.c */
extern uint32_t	placement_address;

/* The kernel's page directory */
struct vm_page_directory *kernel_directory;
/* The current page directory */
struct vm_page_directory *current_directory;

/* Macros used in the bitset algorithms. */
#define INDEX_FROM_BIT(a)	((a) / (8 * 4))
#define OFFSET_FROM_BIT(a)	((a) % (8 * 4))


/* Static function to set a bit in the frames bitset */
static void
set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);

	frames[idx] |= (0x1 << off);
}


/* Static function to clear a bit in the frames bitset */
static void
clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);

	frames[idx] &= ~(0x1 << off);
}


/* Static function to test if a bit is set. */
__attribute__((__unused__))
static uint32_t
test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr / 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);

	return (frames[idx] & (0x1 << off));
}


/* Static function to find the first free frame. */
static uint32_t
first_frame(void)
{
	uint32_t i, j;

	for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] == 0xFFFFFFFF) {
			/* nothing free, exit early. */
			continue;
		}
		/* at least one bit is free here. */
		for (j = 0; j < 32; j++) {
			if ((frames[i] & (0x1 << j)) == 0) {
				/* got it ! */
				return (i * 4 * 8 + j);
			}
		}
	}
	return (-1);
};


/* Function to allocate a frame. */
void
alloc_frame(struct vm_page *p, int is_kernel, int is_writeable)
{
	uint32_t idx;

	if (p->p_frame != 0)
		return; /* Frame was already allocated, return straight away. */

	idx = first_frame(); /* idx is now the index of the first free frame. */
	if (idx == -1)
		PANIC("No free frame.");

	set_frame(idx * 0x1000); /* this frame is now ours! */
	p->p_present = 1; /* Mark it as present. */
	p->p_frame = idx;
	p->p_rw    = (is_writeable) ? 1 : 0; /* Should the page be writeable? */
	p->p_user  = (is_kernel) ? 0 : 1; /* Should the page be user-mode? */
}


/* Function to deallocate a frame. */
void
free_frame(struct vm_page *p)
{

	if (p->p_frame == 0)
		return; /* The given page didn't actually have an allocated frame! */
	clear_frame(p->p_frame); /* Frame is now free again. */
	p->p_frame = 0; /* Page now doesn't have a frame. */
}


void
init_paging(void)
{
	int i;
	uint32_t mem_end_page;
	/* in heap.c */
	extern struct vm_heap *kernel_heap;

	/*
	 * The size of physical memory.
	 * For the moment we assume it is 16MB big.
	 */
	mem_end_page = 0x1000000;

	nframes = mem_end_page / 0x1000;
	frames  = kmalloc0(INDEX_FROM_BIT(nframes));

	/* Let's make a page directory. */
	kernel_directory  = kmalloc0_a(sizeof(struct vm_page_directory));

	/*
	 * Map some pages in the kernel heap area.
	 * Here we call get_page but not alloc_frame. This causes page_table_t's
	 * to be created where necessary. We can't allocate frames yet because
	 * they they need to be identity mapped first below, and yet we can't
	 * increase placement_address between identity mapping and enabling the
	 * heap.
	 */
	i = 0;
	for (i = VM_KERN_HEAP_START; i < VM_KERN_HEAP_START + VM_KERN_HEAP_INITIAL_SIZE; i += 0x1000)
		get_page(i, 1, kernel_directory);

	/*
	 * We need to identity map (phys addr = virt addr) from 0x0 to the end
	 * of used memory, so we can access this transparently, as if paging
	 * wasn't enabled.  NOTE that we use a while loop here deliberately.
	 * inside the loop body we actually change placement_address by calling
	 * kmalloc(). A while loop causes this to be computed on-the-fly rather
	 * than once at the start.
	 */
	i = 0;
	while (i < placement_address + sizeof(struct vm_heap)) {
		/* Kernel code is readable but not writeable from userspace. */
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}

	// Now allocate those pages we mapped earlier.
	for (i = VM_KERN_HEAP_START; i < VM_KERN_HEAP_START + VM_KERN_HEAP_INITIAL_SIZE; i += 0x1000)
		alloc_frame(get_page(i, 0, kernel_directory), 0, 0);

	/* Before we enable paging, we must register our page fault handler. */
	register_interrupt_handler(14, page_fault_handler);

	/* Now, enable paging! */
	switch_page_directory(kernel_directory);

	// Initialise the kernel heap.
	kernel_heap = new_heap(VM_KERN_HEAP_START, VM_KERN_HEAP_START + VM_KERN_HEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}

void
switch_page_directory(struct vm_page_directory *dir)
{
	uint32_t cr0;

	current_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->pd_tblphys));
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; /* Enable paging! */
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}


struct vm_page *
get_page(uint32_t address, int create, struct vm_page_directory *dir)
{
	/* Turn the address into an index. */
	address /= 0x1000;
	/* Find the page table containing this address. */
	uint32_t table_idx = address / 1024;
	/* If this table is already assigned */
	if (dir->pd_tables[table_idx] != NULL)
		return (&dir->pd_tables[table_idx]->pt_pages[address % 1024]);
	else if (create) {
		uint32_t tmp;
		dir->pd_tables[table_idx] = kmalloc0_ap(sizeof(struct vm_page_table), &tmp);
		dir->pd_tblphys[table_idx] = tmp | 0x7; /* PRESENT, RW, US. */
		return (&dir->pd_tables[table_idx]->pt_pages[address % 1024]);
	} else
		return (0);
}


void
page_fault_handler(struct cpu_regs regs)
{
	uint32_t faulting_address;
	int present, rw, us, reserved, id;

	/* The faulting address is stored in the CR2 register. */
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	/* The error code gives us details of what happened. */
	present = !(regs.err_code & 0x1); /* Page not present */
	rw = regs.err_code & 0x2;         /* Write operation? */
	us = regs.err_code & 0x4;         /* Processor was in user-mode? */
	reserved = regs.err_code & 0x8;   /* Overwritten CPU-reserved bits of page entry? */
	id = regs.err_code & 0x10;        /* Caused by an instruction fetch? */

	/* Output an error message. */
	(void)printf("Page fault (");
	if (present)
		(void)printf("present ");
	if (rw)
		(void)printf("read-only ");
	if (us)
		(void)printf("user-mode ");
	if (reserved)
		(void)printf("reserved ");
	(void)printf(") at %x\n", faulting_address);
	PANIC("Page fault");
}
