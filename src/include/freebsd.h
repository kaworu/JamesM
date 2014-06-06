#ifndef FREEBSD_H
#define FREEBSD_H
/*
 * freebsd.h
 *
 * routines stolen to freebsd.
 */
#include <stdarg.h>
#include <sys/queue.h>

int	printf(const char *fmt, ...);
int	vprintf(const char *fmt, va_list ap);

#endif /* ndef FREEBSD_H */
