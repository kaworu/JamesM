/*
 * main.c -- Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */
#include <monitor.h>

struct multiboot;

int
kern_main(struct multiboot *mboot_ptr)
{
	mon_clear();
	(void)printf("Hello World :)\n");
	(void)printf("print test: char(%c), string(%s), int(%d), uint(%u), hex(%x)\n",
	    '9', "test", -42, 42, 17);

	return (0xDEADBABE);
}
