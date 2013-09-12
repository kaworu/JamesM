#ifndef COMMON_H
#define COMMON_H
/*
 * common.h -- Defines typedefs and some global functions.
 * From JamesM's kernel development tutorials.
 */

#define countof(x)	(sizeof((x)) / sizeof((x)[0]))
#define NULL		((void *)0)

#define __packed	__attribute__((__packed__))

typedef unsigned int	uint32_t;
typedef int		int32_t;
typedef unsigned short	uint16_t;
typedef short		int16_t;
typedef unsigned char	uint8_t;
typedef char		int8_t;


void		outb(uint16_t port, uint8_t value); /* write a byte out to port */
uint8_t		inb(uint16_t port); /* read a byte out from port */
uint16_t	inw(uint16_t port); /* read two bytes out from port */


#define PANIC(s)	do {       \
	panic(s);                  \
	} while (/*CONSTCOND*/0);
void	panic(const char *s);

#endif /* ndef COMMON_H */
#include <freebsd.h>
