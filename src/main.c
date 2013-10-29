/*
 * main.c -- Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */
#include <common.h>
#include <descriptor_tables.h>
#include <monitor.h>
#include <paging.h>
#include <timer.h>

#include <heap.h>

struct multiboot;

int
kern_main(struct multiboot *mboot_ptr)
{

	mon_clear();
	(void)printf("+ booting.\n");

	(void)printf("+ descriptor tables init...");
	init_descriptor_tables();
	printf("OK\n");

	void *a = kmalloc(8);

	(void)printf("+ switching to paged mode...");
	init_paging();
	printf("OK\n");

	(void)printf("Hello paginated world :)\n");

	void *b = kmalloc(8);
	void *c = kmalloc(8);
	(void)printf("a=0x%x,b=0x%x,c=0x%x\n", a, b, c);
	kfree(c);
	kfree(b);
	void *d = kmalloc(12);
	(void)printf("d=0x%x", d);

	PANIC("end of kern_main()");
	/* NOTREACHED */
	return (42);
}
