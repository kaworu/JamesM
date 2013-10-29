#ifndef INCLUDE_HEAP_H
#define INCLUDE_HEAP_H
// heap.h -- Interface for kernel heap functions, also provides
//            a placement malloc() for use before the heap is
//            initialised.
//            Written for JamesM's kernel development tutorials.
//
#include <common.h>
#include <sorted_array.h>

#define	VM_KERN_HEAP_START		0xC0000000
#define	VM_KERN_HEAP_INITIAL_SIZE	0x100000
#define	VM_HEAP_INDEX_SIZE		0x20000
#define	VM_HEAP_HDR_MAGIC		0x123890AB
#define	VM_HEAP_FTR_MAGIC		0xBA098321
#define	VM_HEAP_MIN_SIZE		0x70000

/*
 * Size information for a hole/block
 */
struct vm_heap_header {
	uint32_t	hh_magic;   /* Magic number VM_HEAP_HDR_MAGIC, used for
				       error checking and identification. */
	int		hh_is_hole; /* 1 if this is a hole. 0 if this is a
			               block. */
	size_t		hh_size;    /* size of the block, including the end
			               footer. */
};

struct vm_heap_footer {
	uint32_t	 hf_magic;  /* Magic number VM_HEAP_FTR_MAGIC. */
	struct vm_heap_header	*hf_header; /* Pointer to the block header. */
};

struct vm_heap {
	struct sorted_array	h_index;
	uint32_t		h_addr_start; /* The start of our allocated
						 space. */
	uint32_t		h_addr_end;   /* The end of our allocated space.
				                 May be expanded up to
						 max_address. */
	uint32_t		h_addr_max;   /* The maximum address the heap
				                 can be expanded to. */
	int			h_su;         /* Should extra pages requested by
				                 us be mapped as
				                 supervisor-only? */
	int			h_ro;         /* Should extra pages requested by
				                 us be mapped as read-only? */
};


/*
 * Create a new heap.
 */
struct vm_heap	*new_heap(uint32_t start, uint32_t end, uint32_t max, int su, int ro);

/*
 * Allocates a contiguous region of memory 'size' in size. If page_align==1, it creates that block starting
 * on a page boundary.
 */
void	*alloc(uint32_t size, int page_align, struct vm_heap *heap);

/*
 * Releases a block allocated with '_alloc'.
 */
void free(void *p, struct vm_heap *heap);
#endif /* ndef INCLUDE_HEAP_H */

