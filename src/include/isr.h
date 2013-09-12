#ifndef ISR_H
#define ISR_H
/*
 * isr.h -- Interface and structures for high level interrupt service routines.
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten for JamesM's kernel development tutorials.
 */
#include <common.h>


struct cpu_registers
{
   uint32_t	ds;                                     /* Data segment selector */
   uint32_t	edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   uint32_t	int_no, err_code;                       /* Interrupt number and error code (if applicable) */
   uint32_t	eip, cs, eflags, useresp, ss;           /* Pushed by the processor automatically. */
};

#endif /* ndef ISR_H */
