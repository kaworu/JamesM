/*
 * kmalloc.c -- Heap memory allocation routines
 * From JamesM's kernel development tutorials.
 */
#include <null.h>
#include <types.h>
#include <common.h>
#include <kmalloc.h>


// end is defined in the linker script.
extern uint32_t __end;
uint32_t placement_address = (uint32_t)&__end;


/* internal allocation routine */
static void *	_kmalloc(size_t len, int aligned, uint32_t *phys);


void *
kmalloc(size_t len)
{
	return (_kmalloc(len, 0, NULL));
}

void *
kmalloc_a(size_t len)
{

	return (_kmalloc(len, 1, NULL));
}

void *
kmalloc_p(size_t len, uint32_t *phys)
{

	return (_kmalloc(len, 0, phys));
}

void *
kmalloc_ap(size_t len, uint32_t *phys)
{

	return (_kmalloc(len, 1, phys));
}


static void *
_kmalloc(size_t len, int aligned, uint32_t *phys)
{
	if (aligned && placement_address & 0xFFFFF000) {
		/* the address is not already aligned */
		placement_address = (placement_address & 0xFFFFF000) + 0x1000;
	}
	if (phys != NULL)
		*phys = placement_address;

	bzero((void *)placement_address, len);
	placement_address += len;
	return ((void *)(placement_address - len));
}
