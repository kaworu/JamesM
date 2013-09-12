/*
 * main.c -- Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */
#include <descriptor_tables.h>
#include <monitor.h>

struct multiboot;

int
kern_main(struct multiboot *mboot_ptr)
{

	mon_clear();
	init_descriptor_tables();

	(void)printf("Hello World :)\n");
	(void)printf("print test: char(%c), string(%s), int(%d), uint(%u), hex(%x)\n",
	    '9', "test", -42, 42, 17);

	asm volatile ("int $0x3");
	asm volatile ("int $0x4");

	PANIC("end of kern_main()");
	/* NOTREACHED */
	return (42);
}
