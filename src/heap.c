// heap.c -- Kernel heap functions, also provides a placement malloc() for use
// before the heap is initialised.
//           Written for JamesM's kernel development tutorials.

#include <paging.h>
#include <heap.h>


/* the kernel's heap */
struct vm_heap *kernel_heap;

/* in paging.c */
extern struct vm_page_directory *kernel_directory;


static int32_t
find_smallest_hole(size_t size, int page_align, struct vm_heap *heap)
{
	// Find the smallest hole that will fit.
	uint32_t i = 0;
	while (i < heap->h_index.sa_size) {
		struct vm_heap_header *hdr = lookup_sorted_array(&heap->h_index, i);
		// If the user has requested the memory be page-aligned
		if (page_align) {
			// Page-align the starting point of this header.
			uint32_t location = (uint32_t)hdr;
			int32_t offset = 0;
			if ((location + sizeof(struct vm_heap_header)) & 0xFFFFF000)
				offset = 0x1000 /* page size */  - (location + sizeof(struct vm_heap_header)) % 0x1000;
			int32_t hole_size = (int32_t)hdr->hh_size - offset;
			// Can we fit now?
			if (hole_size >= (int32_t)size)
				break;
		}
		else if (hdr->hh_size >= size)
			break;
		i++;
	}
	// Why did the loop exit?
	if (i == heap->h_index.sa_size)
		i = -1; // We got to the end and didn't find anything.

	return (i);
}


static int
vm_heap_header_cmp(void *a, void *b)
{
	return (
	    ((struct vm_heap_header *)a)->hh_size -
	    ((struct vm_heap_header *)b)->hh_size
	);
}


struct vm_heap *
new_heap(uint32_t start, uint32_t end, uint32_t max, int su, int ro)
{
	struct vm_heap *heap = kmalloc(sizeof(struct vm_heap));

	if (heap == NULL)
		PANIC("kmalloc");

	// All our assumptions are made on startAddress and endAddress being
	// page-aligned.
	KASSERT("start of the heap is page aligned", (start % 0x1000) == 0);
	KASSERT("end of the heap is page aligned", (end % 0x1000) == 0);

	// Initialise the index.
	heap->h_index = place_sorted_array((void *)start, VM_HEAP_INDEX_SIZE,
	    &vm_heap_header_cmp);

	// Shift the start address forward to resemble where we can start putting data.
	start += sizeof(void *) * VM_HEAP_INDEX_SIZE;

	// Make sure the start address is page-aligned.
	if (start & 0xFFFFF000) {
		start &= 0xFFFFF000;
		start += 0x1000;
	}
	// Write the start, end and max addresses into the heap structure.
	heap->h_addr_start = start;
	heap->h_addr_end = end;
	heap->h_addr_max = max;
	heap->h_su = su;
	heap->h_ro = ro;

	// We start off with one large hole in the index.
	struct vm_heap_header *hole = (void *)start;
	hole->hh_size = end - start;
	hole->hh_magic = VM_HEAP_HDR_MAGIC;
	hole->hh_is_hole = 1;
	insert_sorted_array(&heap->h_index, hole);

	return (heap);
}


static void
expand(size_t new_size, struct vm_heap *heap)
{
	// Sanity check.
	KASSERT("expand to a greater size", new_size > (heap->h_addr_end - heap->h_addr_start));
	// Get the nearest following page boundary.
	if (new_size & 0xFFFFF000) {
		new_size &= 0xFFFFF000;
		new_size += 0x1000;
	}
	// Make sure we are not overreaching ourselves.
	KASSERT("don't overflow the heap", heap->h_addr_start + new_size <= heap->h_addr_max);

	// This should always be on a page boundary.
	uint32_t old_size = heap->h_addr_end - heap->h_addr_start;
	uint32_t i = old_size;
	while (i < new_size) {
		alloc_frame(get_page(heap->h_addr_start + i, 1, kernel_directory),
		heap->h_su, heap->h_ro);
		i += 0x1000 /* page size */;
	}
	heap->h_addr_end = heap->h_addr_start + new_size;
}


static uint32_t
contract(uint32_t new_size, struct vm_heap *heap)
{
	// Sanity check.
	KASSERT("expand contract a smaller size", new_size < (heap->h_addr_end - heap->h_addr_start));
	// Get the nearest following page boundary.
	if (new_size & 0x1000) {
		new_size &= 0x1000;
		new_size += 0x1000;
	}

	// Don't contract too far!
	if (new_size < VM_HEAP_MIN_SIZE)
		new_size = VM_HEAP_MIN_SIZE;

	uint32_t old_size = heap->h_addr_end - heap->h_addr_start;
	uint32_t i = old_size - 0x1000;
	while (new_size < i) {
		free_frame(get_page(heap->h_addr_start + i, 0, kernel_directory));
		i -= 0x1000 /* page size */;
	}
	heap->h_addr_end = heap->h_addr_start + new_size;

	return (new_size);
}


void *
alloc(uint32_t size, int page_align, struct vm_heap *heap)
{
	// Make sure we take the size of header/footer into account.
	size_t new_size = size + sizeof(struct vm_heap_header) + sizeof(struct vm_heap_footer);
	// Find the smallest hole that will fit.
	int i = find_smallest_hole(new_size, page_align, heap);

	if (i == -1) { // If we didn't find a suitable hole
		// Save some previous data.
		uint32_t old_length = heap->h_addr_end - heap->h_addr_start;
		uint32_t old_end_address = heap->h_addr_end;

		// We need to allocate some more space.
		expand(old_length + new_size, heap);
		uint32_t new_length = heap->h_addr_end - heap->h_addr_start;

		// Find the endmost header. (Not endmost in size, but in location).
		i = 0;
		// Vars to hold the index of, and value of, the endmost header found so far.
		uint32_t idx = -1; uint32_t value = 0x0;
		while (i < heap->h_index.sa_size) {
			uint32_t tmp = (uint32_t)lookup_sorted_array(&heap->h_index, i);
			if (tmp > value) {
				value = tmp;
				idx = i;
			}
			i++;
		}

		// If we didn't find ANY headers, we need to add one.
		if (idx == -1) {
			struct vm_heap_header *header = (struct vm_heap_header *)old_end_address;
			header->hh_magic = VM_HEAP_HDR_MAGIC;
			header->hh_size = new_length - old_length;
			header->hh_is_hole = 1;
			struct vm_heap_footer *footer =
			    (struct vm_heap_footer *) (old_end_address +
			    header->hh_size - sizeof(struct vm_heap_footer));
			footer->hf_magic = VM_HEAP_FTR_MAGIC;
			footer->hf_header = header;
			insert_sorted_array(&heap->h_index, (void*)header);
		} else {
			// The last header needs adjusting.
			struct vm_heap_header *header = lookup_sorted_array(&heap->h_index, idx);
			header->hh_size += new_length - old_length;
			// Rewrite the footer.
			struct vm_heap_footer *footer =
			    (struct vm_heap_footer *) ((uint32_t)header +
			    header->hh_size - sizeof(struct vm_heap_footer));
			footer->hf_magic = VM_HEAP_FTR_MAGIC;
			footer->hf_header = header;
		}
		// We now have enough space. Recurse, and call the function again.
		return (alloc(size, page_align, heap));
	}
	struct vm_heap_header *orig_hole_header = lookup_sorted_array(&heap->h_index, i);
	uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
	uint32_t orig_hole_size = orig_hole_header->hh_size;
	// Here we work out if we should split the hole we found into two parts.
	// Is the original hole size - requested hole size less than the overhead for adding a new hole?
	if ((orig_hole_size - new_size) < (sizeof(struct vm_heap_header) + sizeof(struct vm_heap_footer))) {
		// Then just increase the requested size to the size of the hole we found.
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}
	// If we need to page-align the data, do it now and make a new hole in front of our block.
	if (page_align && (orig_hole_pos & 0xFFFFF000)) {
		uint32_t new_location = orig_hole_pos + 0x1000 /* page size */ -
		    (orig_hole_pos & 0xFFF) - sizeof(struct vm_heap_header);
		struct vm_heap_header *hole_header = (struct vm_heap_header *)orig_hole_pos;
		hole_header->hh_size  = 0x1000 /* page size */ -
		    (orig_hole_pos & 0xFFF) - sizeof(struct vm_heap_header);
		hole_header->hh_magic = VM_HEAP_HDR_MAGIC;
		hole_header->hh_is_hole  = 1;
		struct vm_heap_footer *hole_footer = (struct vm_heap_footer *)
		    ((uint32_t)new_location - sizeof(struct vm_heap_footer));
		hole_footer->hf_magic  = VM_HEAP_FTR_MAGIC;
		hole_footer->hf_header = hole_header;
		orig_hole_pos         = new_location;
		orig_hole_size        = orig_hole_size - hole_header->hh_size;
	} else { // we don't need this hole any more, delete it from the index.
		remove_sorted_array(&heap->h_index, i);
	}
	// Overwrite the original header...
	struct vm_heap_header *block_header =
	    (struct vm_heap_header *)orig_hole_pos;
	block_header->hh_magic   = VM_HEAP_HDR_MAGIC;
	block_header->hh_is_hole = 0;
	block_header->hh_size    = new_size;
	// ...And the footer
	struct vm_heap_footer *block_footer  = (struct vm_heap_footer *)
	    (orig_hole_pos + sizeof(struct vm_heap_header) + size);
	block_footer->hf_magic  = VM_HEAP_FTR_MAGIC;
	block_footer->hf_header = block_header;
	// We may need to write a new hole after the allocated block.
	// We do this only if the new hole would have positive size...
	if (orig_hole_size - new_size > 0) {
		struct vm_heap_header *hole_header = (struct vm_heap_header *)
		    (orig_hole_pos + sizeof(struct vm_heap_header) + size +
		    sizeof(struct vm_heap_footer));
		hole_header->hh_magic    = VM_HEAP_HDR_MAGIC;
		hole_header->hh_is_hole  = 1;
		hole_header->hh_size     = orig_hole_size - new_size;
		struct vm_heap_footer *hole_footer = (struct vm_heap_footer *)
		    ((uint32_t)hole_header + orig_hole_size - new_size -
		     sizeof(struct vm_heap_footer));
		if ((uint32_t)hole_footer < heap->h_addr_end) {
			hole_footer->hf_magic = VM_HEAP_FTR_MAGIC;
			hole_footer->hf_header = hole_header;
		}
		// Put the new hole in the index;
		insert_sorted_array(&heap->h_index, (void*)hole_header);
	}
	// ...And we're done!
	return (void *)((uint32_t)block_header + sizeof(struct vm_heap_header));
}


void
free(void *p, struct vm_heap *heap)
{
	// Exit gracefully for null pointers.
	if (p == NULL || heap == NULL)
		return;

	// Get the header and footer associated with this pointer.
	struct vm_heap_header *header = (struct vm_heap_header*) ((uint32_t)p -
	    sizeof(struct vm_heap_header));
	struct vm_heap_footer *footer = (struct vm_heap_footer*)
	    ((uint32_t)header + header->hh_size - sizeof(struct vm_heap_footer));

	// Sanity checks.
	KASSERT("header magic match", header->hh_magic == VM_HEAP_HDR_MAGIC);
	KASSERT("footer magic match", footer->hf_magic == VM_HEAP_FTR_MAGIC);

	// Make us a hole.
	header->hh_is_hole = 1;
	// Do we want to add this header into the 'free holes' index?
	char do_add = 1;

	// Unify left
	// If the thing immediately to the left of us is a footer...
	struct vm_heap_footer *test_footer = (struct vm_heap_footer*)
	    ((uint32_t)header - sizeof(struct vm_heap_footer));
	if (test_footer->hf_magic == VM_HEAP_FTR_MAGIC &&
	    test_footer->hf_header->hh_is_hole == 1) {
		uint32_t cache_size = header->hh_size; // Cache our current size.
		header = test_footer->hf_header;     // Rewrite our header with the new one.
		footer->hf_header = header;          // Rewrite our footer to point to the new header.
		header->hh_size += cache_size;       // Change the size.
		do_add = 0;                       // Since this header is already in the index, we don't want to add it again.
	}

	// Unify right
	// If the thing immediately to the right of us is a header...
	struct vm_heap_header *test_header = (struct vm_heap_header*)
	    ((uint32_t)footer + sizeof(struct vm_heap_footer));
	if (test_header->hh_magic == VM_HEAP_HDR_MAGIC &&
	    test_header->hh_is_hole) {
		header->hh_size += test_header->hh_size; // Increase our size.
		test_footer = (struct vm_heap_footer*) // Rewrite it's footer to point to our header.
		    ((uint32_t)test_header +
		    test_header->hh_size - sizeof(struct vm_heap_footer));
		footer = test_footer;
		// Find and remove this header from the index.
		uint32_t i = 0;
		while (i < heap->h_index.sa_size &&
		    lookup_sorted_array(&heap->h_index, i) != test_header)
			i++;

		// Make sure we actually found the item.
		KASSERT("found an item while unifying right", i < heap->h_index.sa_size);
		// Remove it.
		remove_sorted_array(&heap->h_index, i);
	}

	// If the footer location is the end address, we can contract.
	if ((uint32_t)footer + sizeof(struct vm_heap_footer) ==
	    heap->h_addr_end) {
		uint32_t old_length = heap->h_addr_end - heap->h_addr_start;
		uint32_t new_length =
		    contract((uint32_t)header - heap->h_addr_start, heap);
		// Check how big we will be after resizing.
		if (header->hh_size - (old_length-new_length) > 0) {
			// We will still exist, so resize us.
			header->hh_size -= old_length - new_length;
			footer = (struct vm_heap_footer*)
			    ((uint32_t)header + header->hh_size -
			    sizeof(struct vm_heap_footer));
			footer->hf_magic = VM_HEAP_FTR_MAGIC;
			footer->hf_header = header;
		} else {
			// We will no longer exist :(. Remove us from the index.
			uint32_t i = 0;
			while (i < heap->h_index.sa_size &&
			    lookup_sorted_array(&heap->h_index, i) != test_header)
				i++;
			// If we didn't find ourselves, we have nothing to remove.
			if (i < heap->h_index.sa_size)
				remove_sorted_array(&heap->h_index, i);
		}
	}
	if (do_add)
		insert_sorted_array(&heap->h_index, header);
}
