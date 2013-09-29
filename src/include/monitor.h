#ifndef MONITOR_H
#define MONITOR_H
/*
 * monitor.h -- Defines the interface for monitor.h
 * From JamesM's kernel development tutorials.
 */
#include <common.h>


void	mon_putchar(char c); /* Write a single character out to the screen. */
void	mon_clear(void); /* Clear the screen. */
void	mon_write(char *s); /* Output a null-terminated ASCII string to the monitor. */

#endif /* ndef MONITOR_H */
