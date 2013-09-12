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
	(void)printf("Hello World :)");

	return (0xDEADBABE);
}
