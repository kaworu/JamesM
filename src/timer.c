/*
 * timer.c -- Initialises the PIT, and handles clock updates.
 * Written for JamesM's kernel development tutorials.
 */
#include <common.h>
#include <timer.h>
#include <isr.h>
#include <monitor.h>


static void
timer_callback(struct cpu_regs regs)
{
	static uint32_t tick;

	(void)printf("Tick: %d\n", ++tick);
}

void init_timer(uint32_t freq)
{
	uint32_t div;
	uint8_t lo, hi;

	/* register our timer callback. */
	register_interrupt_handler(IRQ0, &timer_callback);

	/*
	 * The value we send to the PIT is the value to divide it's input clock
	 * (1193180 Hz) by, to get our required frequency. Important to note is
	 * that the divisor must be small enough to fit into 16-bits.
	 */
	div = 1193180 / freq;

	/* Send the command byte. */
	outb(0x43, 0x36);

	/* Divisor has to be sent byte-wise, so split here into upper/lower
	   bytes. */
	lo = (uint8_t)(div & 0xFF);
	hi = (uint8_t)((div >> 8) & 0xFF);

	// Send the frequency divisor.
	outb(0x40, lo);
	outb(0x40, hi);
}
