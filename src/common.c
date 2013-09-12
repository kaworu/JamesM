/*
 * common.c -- Defines some global functions.
 * From JamesM's kernel development tutorials.
 */
#include <stdarg.h>

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
_panic(const char *fmt, ...)
{
	va_list ap;

	(void)printf("\n***** Kernel panic! *****\n");

	va_start(ap, fmt);
	(void)vprintf(fmt, ap);
	va_end(ap);

	(void)printf("\n*************************\n");

	for (;;)
		;
}
