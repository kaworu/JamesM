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


#define PANIC(s, ...)	_panic("%s:%u in %s: " s, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
void	_panic(const char *fmt, ...);

#define KASSERT(msg, cond) (                                       \
		(cond) ?                                           \
		(void)0 :                                          \
		PANIC("assertion failed: %s (%s)", (msg), (#cond)) \
)

void	bzero(void *b, size_t len);
void *	memset(void *b, int c, size_t len);

#include <kmalloc.h>
#include <freebsd.h>
#endif /* ndef COMMON_H */
