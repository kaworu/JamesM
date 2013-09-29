#ifndef PAGING_H
#define PAGING_H

#include <common.h>
#include <isr.h>


struct vm_page {
	uint32_t p_present  : 1;   /* Page present in memory */
	uint32_t p_rw       : 1;   /* Read-only if clear, readwrite if set */
	uint32_t p_user     : 1;   /* Supervisor level only if clear */
	uint32_t p_accessed : 1;   /* Has the page been accessed since last refresh? */
	uint32_t p_dirty    : 1;   /* Has the page been written to since last refresh? */
	uint32_t p_unused   : 7;   /* Amalgamation of unused and reserved bits */
	uint32_t p_frame    : 20;  /* Frame address (shifted right 12 bits) */
};

struct vm_page_table {
   struct vm_page	pt_pages[1024];
};

struct vm_page_directory {
	/*
	 * Array of pointers to pagetables.
	 */
	struct vm_page_table *pd_tables[1024];
	/*
	 * Array of pointers to the pagetables above, but gives their *physical*
	 * location, for loading into the CR3 register.
	 */
	uint32_t pd_tblphys[1024];
	/*
	 * The physical address of pd_tblphys. This comes into play
	 * when we get our kernel heap allocated and the directory
	 * may be in a different location in virtual memory.
	 */
	uint32_t pd_tblphys_addr;
};

/*
 * Sets up the environment, page directories etc and enables paging.
 */
void	init_paging(void);

/*
 *Causes the specified page directory to be loaded into the CR3 register.
 */
void	switch_page_directory(struct vm_page_directory *pd);

/*
 * Retrieves a pointer to the page required. If create == 1, if the page-table in
 * which this page should reside isn't created, create it!
 */
struct vm_page	*get_page(uint32_t address, int create,
		    struct vm_page_directory *dir);

/**
 * Page faults handler.
 */
void page_fault_handler(struct cpu_regs regs);
#endif /* ndef PAGING_H */
