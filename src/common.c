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


void *
memset(void *b, int c, size_t len)
{
	char *bb;

	for (bb = b; len > 0; len--)
		*bb++ = c;

	return (b);
}


void
bzero(void *b, size_t len)
{

	(void)memset(b, 0, len);
}


void *
memcpy(void *dest, const void *src, size_t count)
{
	char* dst8 = (char *)dest;
	char* src8 = (char *)src;

	while (count--)
		*dst8++ = *src8++;

	return dest;
}


size_t
strlen(const char *s)
{
	size_t l = 0;
	for (l = 0; s[l] != '\0'; l++)
		;
	return (l);
}

int
strcmp(const char *s1, const char *s2)
{
	// stolen from freebsd
while (*s1 == *s2++)
if (*s1++ == '\0')
return (0);
return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));

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
	/* NOTREACHED */
}
