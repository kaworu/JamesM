/*
 * main.c -- Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */
#include <descriptor_tables.h>
#include <monitor.h>
#include <timer.h>

struct multiboot;

int
kern_main(struct multiboot *mboot_ptr)
{

	mon_clear();
	init_descriptor_tables();
	init_timer(100);

	(void)printf("Hello World :)\n");

	asm volatile ("sti");
	init_timer(100);

	PANIC("end of kern_main()");
	/* NOTREACHED */
	return (42);
}
