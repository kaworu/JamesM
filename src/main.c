/*
 * src/main.c
 *
 * Defines the C-code kernel entry point, calls initialisation routines.
 * Made for JamesM's tutorials.
 */

struct multiboot;

int
kern_main(struct multiboot *mboot_ptr)
{
	// All our initialisation calls will go in here.
	return (0xDEADBABE);
}
