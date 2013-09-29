/*
 * main.c -- Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */
#include <common.h>
#include <descriptor_tables.h>
#include <monitor.h>
#include <paging.h>
#include <timer.h>

struct multiboot;

int
kern_main(struct multiboot *mboot_ptr)
{
	uint32_t *ptr, pg_fault;

	init_descriptor_tables();
	mon_clear();
	init_paging();
	(void)printf("Hello paginated world :)\n");

	ptr = (uint32_t *)0xA0000000;
	pg_fault = *ptr;

	PANIC("end of kern_main()");
	/* NOTREACHED */
	return (42);
}
