/*
 * isr.c -- High level interrupt service routines and interrupt request handlers.
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten for JamesM's kernel development tutorials.
 */
#include <common.h>
#include <monitor.h>
#include <isr.h>

/* This gets called from our ASM interrupt handler stub. */
void isr_handler(struct cpu_registers regs)
{

	(void)printf("recieved interrupt: %d\n", regs.int_no);
}
