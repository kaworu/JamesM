#ifndef TIMER_H
#define TIMER_H
/*
 * timer.h -- Defines the interface for all PIT-related functions.
 * Written for JamesM's kernel development tutorials.
 */
#include <common.h>

void	timer_init(uint32_t freq);

#endif /* ndef TIMER_H */
