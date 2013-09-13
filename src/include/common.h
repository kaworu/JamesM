#ifndef COMMON_H
#define COMMON_H
/*
 * common.h -- Defines typedefs and some global functions.
 * From JamesM's kernel development tutorials.
 */
#include <null.h>
#include <types.h>
#include <__attributes__.h>

#define countof(x)	(sizeof((x)) / sizeof((x)[0]))

void		outb(uint16_t port, uint8_t value); /* write a byte out to port */
uint8_t		inb(uint16_t port); /* read a byte out from port */
uint16_t	inw(uint16_t port); /* read two bytes out from port */


#define PANIC(s, ...)	do {                                    \
	_panic("%s:%u: " s, __FILE__, __LINE__, ##__VA_ARGS__); \
} while (/*CONSTCOND*/0);
void	_panic(const char *fmt, ...);

void	bzero(void *b, size_t len);
void *	memset(void *b, int c, size_t len);

#endif /* ndef COMMON_H */
#include <freebsd.h>
