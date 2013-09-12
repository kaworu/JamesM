/*-
 * Copyright (c) 1998 Robert Nordier
 * All rights reserved.
 * Copyright (c) 2006 M. Warner Losh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are freely
 * permitted provided that the above copyright notice and this
 * paragraph and the following disclaimer are duplicated in all
 * such forms.
 *
 * This software is provided "AS IS" and without any express or
 * implied warranties, including, without limitation, the implied
 * warranties of merchantability and fitness for a particular
 * purpose.
 *
 * $FreeBSD$
 */

#include <stdarg.h>
#include <monitor.h>

int
printf(const char *fmt,...)
{
	va_list ap;
	const char *hex = "0123456789abcdef";
	char buf[10];
	char *s;
	unsigned u;
	int i = 0, c;

	va_start(ap, fmt);
	while ((c = *fmt++)) {
		if (c == '%') {
			c = *fmt++;
			switch (c) {
			case 'c':
				mon_putchar(va_arg(ap, int));
				i++;
				continue;
			case 's':
				for (s = va_arg(ap, char *); *s; s++) {
					mon_putchar(*s);
					i++;
				}
				continue;
			case 'd':	/* A lie, always prints unsigned */
			case 'u':
				u = va_arg(ap, unsigned);
				s = buf;
				do
					*s++ = '0' + u % 10U;
				while (u /= 10U);
			dumpbuf:;
				while (--s >= buf) {
					mon_putchar(*s);
					i++;
				}
				continue;
			case 'x':
				u = va_arg(ap, unsigned);
				s = buf;
				do
					*s++ = hex[u & 0xfu];
				while (u >>= 4);
				goto dumpbuf;
			}
		}
		mon_putchar(c);
		i++;
	}
	va_end(ap);

	return (i);
}
