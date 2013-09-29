/*
 * isr.c -- High level interrupt service routines and interrupt request handlers.
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten for JamesM's kernel development tutorials.
 */
#include <common.h>
#include <monitor.h>
#include <isr.h>

isrhdl_t interrupt_handlers[256];

/* This gets called from our ASM interrupt handler stub. */
void isr_handler(struct cpu_regs regs)
{

	if (interrupt_handlers[regs.int_no] != NULL) {
		isrhdl_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	} else {
		(void)printf("unhandled interrupt: %d\n", regs.int_no);
	}
}

/* This gets called from our ASM interrupt handler stub. */
void irq_handler(struct cpu_regs regs) {
	/* Send an EOI (end of interrupt) signal to the PICs. If this interrupt
	   involved the slave. */
	if (regs.int_no >= 40) {
		/* Send reset signal to slave.*/
		outb(0xA0, 0x20);
	}
	/* Send reset signal to master. */
	outb(0x20, 0x20);

	if (interrupt_handlers[regs.int_no] != 0) {
		isrhdl_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	}
}

void
register_interrupt_handler(uint32_t n, isrhdl_t handler)
{

	if (n > countof(interrupt_handlers))
		PANIC("Bad interrupt handler index.");
	interrupt_handlers[n] = handler;
}
