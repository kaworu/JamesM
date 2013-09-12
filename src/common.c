/*
 * common.c -- Defines some global functions.
 * From JamesM's kernel development tutorials.
 */
#include <common.h>
#include <monitor.h>


void
outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}


uint8_t
inb(uint16_t port)
{
	uint8_t ret;

	asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return (ret);
}


uint16_t
inw(uint16_t port)
{
	uint16_t ret;

	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return (ret);
}


void
panic(const char *s)
{
	(void)printf("*** Kernel Panic: %s ***\n\r", s);
	for (;;)
		;
}
